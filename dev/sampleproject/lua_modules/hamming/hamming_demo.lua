hamming = require("hamming")

-- Start the REPL
repeat
    print("Enter two strings to compare:")
    local str1, str2 = io.read("*l", "*l")
    if str1 and str2 then
        print("'", str1, "', '", str2, "' =", hamming(str1, str2))
    end
until not str1