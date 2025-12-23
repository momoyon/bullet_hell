local angle = 0
local angle2 = 0
local angle_dt = 100
local angle_dt_dt = 100
local bullet_count = 2
local bullet_count2 = 3

function Pattern(x, y, dt)
	local bullets = {}

	for i = 1, bullet_count, 1 do
		local hbox = bullet_hitbox
		local a = angle + (i * (360 / bullet_count))
		local b = Bullet.new(x, y, "bullet.png", 1, 1, 100, 500, 500, -200, a, hbox)
		table.insert(bullets, b)
	end

	for i = 1, bullet_count2, 1 do
		local hbox = bullet_hitbox
		local a = angle2 + (i * (360 / bullet_count2))
		local b = Bullet.new(x, y, "bullet.png", 1, 1, 50, 500, 500, -600, a, hbox)
		table.insert(bullets, b)
	end

	angle = angle + (dt * angle_dt)
	angle_dt = angle_dt + (dt * angle_dt_dt)

	angle2 = angle2 + (dt * 1000)

	return bullets, #bullets
end
