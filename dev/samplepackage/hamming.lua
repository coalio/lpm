-- hamming: A library for computing the Hamming distance between two strings.

local function hamming(str1, str2)
    local count = 0
    for i = 1, #str1 do
        if str1:sub(i, i) ~= str2:sub(i, i) then
            count = count + 1
        end
    end
    return count
end

-- When returning a lpm-based module, you can specify a
-- second argument to specify the way that lpm should treat it.

-- By default (no second arg), the module is returned from the import function.
-- If you specify "env", then the module is loaded into the environment.
-- If you specify "global", then the module is loaded into the global environment,

return hamming, "env"