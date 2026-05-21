cmake_minimum_required(VERSION 3.25)
project(cpp_quant_platform VERSION 0.1.0 LANGUAGES CXX)

option(QP_BUILD_TESTS "Build tests" ON)
option(QP_BUILD_BENCHMARKS "Build dependency-free smoke benchmarks" ON)
option(QP_BUILD_APPS "Build CLI apps" ON)
option(QP_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(QP_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(QP_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

add_library(qp_options INTERFACE)
add_library(qp::options ALIAS qp_options)
target_compile_features(qp_options INTERFACE cxx_std_20)

add_library(qp_warnings INTERFACE)
add_library(qp::warnings ALIAS qp_warnings)
target_compile_options(qp_warnings INTERFACE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wsign-conversion;-Wshadow;-Wnon-virtual-dtor;-Wold-style-cast;-Wformat=2>
  $<$<CXX_COMPILER_ID:MSVC>:/W4;/permissive->
)
if(QP_WARNINGS_AS_ERRORS)
  target_compile_options(qp_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/WX>)
endif()

add_library(qp_sanitizers INTERFACE)
add_library(qp::sanitizers ALIAS qp_sanitizers)
set(QP_SAN_FLAGS "")
if(QP_ENABLE_ASAN)
  list(APPEND QP_SAN_FLAGS -fsanitize=address)
endif()
if(QP_ENABLE_UBSAN)
  list(APPEND QP_SAN_FLAGS -fsanitize=undefined)
endif()
if(QP_SAN_FLAGS)
  target_compile_options(qp_sanitizers INTERFACE $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:${QP_SAN_FLAGS};-fno-omit-frame-pointer>)
  target_link_options(qp_sanitizers INTERFACE $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:${QP_SAN_FLAGS}>)
endif()

add_library(qp_platform
  src/core/error.cpp
  src/core/time.cpp
  src/core/types.cpp
  src/core/diagnostics.cpp
  src/core/config.cpp
  src/schema/schema_version.cpp
  src/schema/validation.cpp
  src/data/instrument.cpp
  src/data/dataset_writer.cpp
  src/data/bar.cpp
  src/data/trade.cpp
  src/data/quote.cpp
  src/data/market_data.cpp
  src/data/csv_reader.cpp
  src/replay/event.cpp
  src/replay/event_source.cpp
  src/replay/replay_clock.cpp
  src/replay/replay_engine.cpp
  src/features/feature.cpp
  src/features/rolling_window.cpp
  src/features/returns.cpp
  src/features/rolling_mean.cpp
  src/features/rolling_variance.cpp
  src/labels/label.cpp
  src/labels/leakage_guard.cpp
  src/labels/forward_return.cpp
  src/labels/triple_barrier.cpp
  src/signals/momentum.cpp
  src/signals/signal.cpp
  src/execution/order.cpp
  src/execution/fill.cpp
  src/execution/commission_model.cpp
  src/execution/slippage_model.cpp
  src/execution/execution_simulator.cpp
  src/portfolio/portfolio.cpp
  src/backtest/simple_backtest.cpp
  src/backtest/backtest_config.cpp
  src/backtest/report.cpp
  src/backtest/metrics.cpp
  src/reporting/report_writer.cpp
)
add_library(qp::platform ALIAS qp_platform)
target_include_directories(qp_platform PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(qp_platform PUBLIC qp::options PRIVATE qp::warnings qp::sanitizers)
target_compile_definitions(qp_platform PUBLIC QP_VERSION="${PROJECT_VERSION}")

if(QP_BUILD_APPS)
  add_executable(qp apps/qp/main.cpp)
  target_link_libraries(qp PRIVATE qp::platform qp::warnings qp::sanitizers)
endif()

include(CTest)
if(QP_BUILD_TESTS)
  enable_testing()
  add_executable(qp_tests tests/test_main.cpp tests/test_execution_reporting.cpp tests/test_schema_artifacts.cpp)
  target_link_libraries(qp_tests PRIVATE qp::platform qp::warnings qp::sanitizers)
  add_test(NAME qp.unit COMMAND qp_tests)
  if(QP_BUILD_APPS)
    add_test(NAME qp.cli_help COMMAND qp --help)
    add_test(NAME qp.cli_validate_config COMMAND qp validate-config --config ${PROJECT_SOURCE_DIR}/examples/configs/simple_momentum.flat.json)
    add_test(NAME qp.cli_backtest COMMAND qp backtest --config ${PROJECT_SOURCE_DIR}/examples/configs/simple_momentum.flat.json --output-dir ${PROJECT_BINARY_DIR}/cli_backtest_out)
  endif()
endif()

if(QP_BUILD_BENCHMARKS)
  add_executable(qp_bench_smoke benchmarks/bench_smoke.cpp)
  target_link_libraries(qp_bench_smoke PRIVATE qp::platform qp::warnings qp::sanitizers)
  add_test(NAME qp.benchmark_smoke COMMAND qp_bench_smoke --iterations 1000)
endif()
