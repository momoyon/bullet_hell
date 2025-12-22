function Pattern1(bullets)
	local count = bullets:get_count() -- Call the accessor for count
	print("BULLETS COUNT: " .. count)

	-- local items = bullets:get_items() -- Call the accessor for items
	-- for i = 1, count do
	-- 	print("Bullet ID: " .. items[i]) -- Print each bullet ID
	-- end
end

function Add(a, b)
	print(a + b)
end
