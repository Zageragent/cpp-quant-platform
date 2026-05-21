#include "qp/replay/event.hpp"
#include <variant>
namespace qp::replay { Timestamp event_time(const ReplayEvent& e){ return std::visit([](const auto& x){ if constexpr (requires { x.receive_ts; }) return x.receive_ts; else return x.ts; }, e); } SequenceNumber sequence_number(const ReplayEvent& e){ return std::visit([](const auto& x){ return x.sequence; }, e); } bool event_less(const ReplayEvent& a,const ReplayEvent& b){ if(event_time(a)!=event_time(b)) return event_time(a)<event_time(b); return sequence_number(a)<sequence_number(b); } }
