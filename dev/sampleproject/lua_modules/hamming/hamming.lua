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

return hamming