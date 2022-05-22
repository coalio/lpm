#pragma once
#include <string>
#include <exception>

namespace ErrorHandling {
    std::string what(const std::exception_ptr &eptr = std::current_exception()) {
        if (!eptr) { throw std::bad_exception(); }

        try { std::rethrow_exception(eptr); }
        catch (const std::exception &e) { return e.what()   ; }
        catch (const std::string    &e) { return e          ; }
        catch (const char           *e) { return e          ; }
        catch (...)                     { return "Unknown error"; }
    }
}
