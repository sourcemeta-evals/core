# Used by verify/repro/c12_copy_trace.sh: prints a 12-frame backtrace of every JSON copy-constructor call
set pagination off
break sourcemeta::core::JSON::JSON(const sourcemeta::core::JSON&)
commands
silent
bt 12
continue
end
run
