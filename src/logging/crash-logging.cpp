#include "crash-logging.hpp"

#include "log.hpp"

#include <Windows.h>

namespace gelly {
namespace {
// ReSharper disable once CppParameterMayBeConstPtrOrRef
LONG CrashHandler(_EXCEPTION_POINTERS *info) {
  Log::Error("Crash detected, writing crash dump");

  Log::Error("RIP: 0x{:X}", info->ContextRecord->Rip);
  Log::Error("RSP: 0x{:X}", info->ContextRecord->Rsp);
  Log::Error("RBP: 0x{:X}", info->ContextRecord->Rbp);
  Log::Error("RAX: 0x{:X}", info->ContextRecord->Rax);
  Log::Error("RBX: 0x{:X}", info->ContextRecord->Rbx);
  Log::Error("RCX: 0x{:X}", info->ContextRecord->Rcx);

  switch (info->ExceptionRecord->ExceptionCode) {
  case EXCEPTION_ACCESS_VIOLATION:
    Log::Error("Access violation");
    Log::Error("Attempted to {}", info->ExceptionRecord->ExceptionInformation[0]
                                      ? "write"
                                      : "read");
    Log::Error("Address: 0x{:X}",
               info->ExceptionRecord->ExceptionInformation[1]);
    break;
  case 0xE06D7363: {
    Log::Error("C++ exception");
    const auto exception = reinterpret_cast<std::exception *>(
        info->ExceptionRecord->ExceptionInformation[1]);
    Log::Error("Exception: {}", exception->what());
  } break;
  default:
    Log::Error("Unknown exception code: 0x{:X}",
               info->ExceptionRecord->ExceptionCode);
  }

  Log::SaveToFile();

  return EXCEPTION_CONTINUE_SEARCH;
}
} // namespace

void SetupCrashLogging() { AddVectoredExceptionHandler(1, CrashHandler); }
void ShutdownCrashLogging() { RemoveVectoredExceptionHandler(CrashHandler); }

} // namespace gelly