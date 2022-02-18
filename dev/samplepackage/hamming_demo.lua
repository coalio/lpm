-- This is useful:
import = require("lpm").import

-- Optionally, if your module returns a table, you can do it as follows:
-- local hamming = import "hamming:latest"
import "hamming:latest"

-- If you're very skeptic, you can also use the default require function
-- (Note that this means you lose all of the benefits of using lpm imports)
-- local hamming = require("hamming")

-- Start the REPL
repeat
    print("Enter two strings to compare:")
    local str1, str2 = io.read("*l", "*l")
    if str1 and str2 then
        print("'", str1, "', '", str2, "' =", hamming(str1, str2))
    end
until not str1