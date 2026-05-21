#Requires -Version 5.1
<#
.SYNOPSIS
  Prepare a Windows machine to build this repository in the VS Code Dev Container.

.DESCRIPTION
  Installs/checks the Windows-side prerequisites for the WSL + Docker Desktop + VS Code
  Dev Containers workflow:
    - WSL 2
    - Git
    - Docker Desktop
    - Visual Studio Code
    - VS Code WSL and Dev Containers extensions

  Run from an elevated PowerShell prompt for the first setup, because WSL optional
  features and Docker Desktop may require admin rights and a reboot.

.PARAMETER NoInstall
  Only check prerequisites; do not install missing tools.

.PARAMETER SkipDockerStart
  Do not try to start Docker Desktop after installation/checks.
#>

param(
  [switch]$NoInstall,
  [switch]$SkipDockerStart
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Write-Step([string]$Message) {
  Write-Host "`n==> $Message" -ForegroundColor Cyan
}

function Test-Command([string]$Name) {
  return $null -ne (Get-Command $Name -ErrorAction SilentlyContinue)
}

function Test-IsAdmin {
  $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
  $principal = New-Object Security.Principal.WindowsPrincipal($identity)
  return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Install-WinGetPackage([string]$Id, [string]$DisplayName) {
  if ($NoInstall) {
    Write-Warning "$DisplayName is missing. Re-run without -NoInstall to install it."
    return
  }

  if (-not (Test-Command winget)) {
    throw "winget is required to auto-install $DisplayName. Install App Installer from Microsoft Store, then re-run this script."
  }

  Write-Step "Installing/upgrading $DisplayName with winget"
  winget install --id $Id --exact --source winget --accept-package-agreements --accept-source-agreements
}

function Ensure-Wsl {
  Write-Step "Checking WSL"

  if (Test-Command wsl) {
    try {
      $wslVersion = & wsl --version 2>$null
      if ($LASTEXITCODE -eq 0) {
        $wslVersion | ForEach-Object { Write-Host $_ }
      }
    } catch {
      Write-Host "WSL command exists; version output is unavailable on older Windows builds."
    }
  } elseif ($NoInstall) {
    Write-Warning "WSL is missing."
    return
  } else {
    if (-not (Test-IsAdmin)) {
      throw "WSL installation requires an elevated PowerShell prompt. Re-run PowerShell as Administrator."
    }
    Write-Step "Installing WSL"
    wsl --install
    Write-Warning "WSL requested installation. Reboot Windows if prompted, then re-run this script."
    return
  }

  try {
    & wsl --set-default-version 2 | Out-Host
  } catch {
    Write-Warning "Could not set WSL default version to 2 automatically: $($_.Exception.Message)"
  }

  $distros = @()
  try {
    $distros = & wsl -l -q 2>$null | Where-Object { $_ -and $_.Trim().Length -gt 0 }
  } catch {
    $distros = @()
  }

  if ($distros.Count -eq 0) {
    Write-Warning "No WSL distro is installed yet. Install Ubuntu from Microsoft Store or run: wsl --install -d Ubuntu"
  } else {
    Write-Host "Installed WSL distros: $($distros -join ', ')"
  }
}

function Ensure-Git {
  Write-Step "Checking Git"
  if (Test-Command git) {
    git --version
  } else {
    Install-WinGetPackage -Id "Git.Git" -DisplayName "Git"
  }
}

function Ensure-VSCode {
  Write-Step "Checking Visual Studio Code"
  if (Test-Command code) {
    code --version | Select-Object -First 2 | ForEach-Object { Write-Host $_ }
  } else {
    Install-WinGetPackage -Id "Microsoft.VisualStudioCode" -DisplayName "Visual Studio Code"
  }

  if (Test-Command code) {
    Write-Step "Installing VS Code extensions"
    $extensions = @(
      "ms-vscode-remote.remote-wsl",
      "ms-vscode-remote.remote-containers",
      "ms-vscode.cpptools",
      "ms-vscode.cmake-tools",
      "twxs.cmake",
      "ms-azuretools.vscode-docker"
    )

    foreach ($extension in $extensions) {
      code --install-extension $extension --force | Out-Host
    }
  } else {
    Write-Warning "The 'code' command is still unavailable. Open VS Code once and enable the shell command, or restart PowerShell."
  }
}

function Ensure-DockerDesktop {
  Write-Step "Checking Docker Desktop"

  if (Test-Command docker) {
    docker --version
  } else {
    Install-WinGetPackage -Id "Docker.DockerDesktop" -DisplayName "Docker Desktop"
  }

  if (-not $SkipDockerStart) {
    $dockerDesktopPath = Join-Path $env:ProgramFiles "Docker\Docker\Docker Desktop.exe"
    if (Test-Path $dockerDesktopPath) {
      Write-Step "Starting Docker Desktop"
      Start-Process $dockerDesktopPath | Out-Null
    }
  }

  if (Test-Command docker) {
    Write-Step "Waiting for Docker Engine"
    $deadline = (Get-Date).AddMinutes(4)
    do {
      try {
        docker info *> $null
        if ($LASTEXITCODE -eq 0) {
          docker version --format 'Docker client {{.Client.Version}}, server {{.Server.Version}}'
          return
        }
      } catch {
        # Docker Desktop is still starting.
      }
      Start-Sleep -Seconds 5
    } while ((Get-Date) -lt $deadline)

    Write-Warning "Docker CLI is installed, but the engine is not reachable yet. Open Docker Desktop, finish onboarding, enable WSL integration, then re-run this script."
  } else {
    Write-Warning "Docker CLI is still unavailable. Restart PowerShell or Windows after Docker Desktop installation."
  }
}

function Test-DevContainerFiles {
  Write-Step "Checking repository dev container files"
  $repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
  $devcontainerJson = Join-Path $repoRoot ".devcontainer\devcontainer.json"
  $dockerfile = Join-Path $repoRoot ".devcontainer\Dockerfile"

  if (-not (Test-Path $devcontainerJson)) { throw "Missing $devcontainerJson" }
  if (-not (Test-Path $dockerfile)) { throw "Missing $dockerfile" }

  Get-Content $devcontainerJson -Raw | ConvertFrom-Json | Out-Null
  Write-Host "Found valid .devcontainer/devcontainer.json"
  Write-Host "Found .devcontainer/Dockerfile"
}

Ensure-Wsl
Ensure-Git
Ensure-DockerDesktop
Ensure-VSCode
Test-DevContainerFiles

Write-Step "Next steps"
Write-Host "1. Open Docker Desktop and ensure Settings > Resources > WSL integration is enabled for your Ubuntu distro."
Write-Host "2. From WSL, clone/open this repo:"
Write-Host "   git clone https://github.com/Zageragent/cpp-quant-platform.git"
Write-Host "   cd cpp-quant-platform"
Write-Host "   code ."
Write-Host "3. In VS Code, run: Dev Containers: Reopen in Container"
Write-Host "4. The container will run cmake configure/build/test automatically."
