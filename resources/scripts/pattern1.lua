local angle = 0
local angle2 = 0
local angle3 = 0
local angle_dt = 100
local angle_dt_dt = 100
local bullet_count = 2
local bullet_count2 = 3
local bullet_count3 = 4

function Pattern(x, y, dt)
	local bullets = {}

	for i = 1, bullet_count, 1 do
		local hbox = bullet_hitbox
		local a = angle + (i * (360 / bullet_count))
		local b = Bullet.new(x, y, 0, 0, 32, 32, 1, 1, 100, 500, 500, -200, a, hbox)
		table.insert(bullets, b)
	end

	for i = 1, bullet_count2, 1 do
		local hbox = bullet_hitbox
		local a = angle2 + (i * (360 / bullet_count2))
		local b = Bullet.new(x, y, 32, 0, 32, 32, 1, 1, 50, 500, 500, -600, a, hbox)
		table.insert(bullets, b)
	end

	for i = 1, bullet_count3, 1 do
		local hbox = bullet2_hitbox
		local a = angle3 + (i * (360 / bullet_count3))
		local b = Bullet.new(x, y, 0, 32, 11, 16, 1, 1, 0, 500, 0, 100, a, hbox)
		table.insert(bullets, b)
	end

	angle = angle + (dt * angle_dt)
	angle_dt = angle_dt + (dt * angle_dt_dt)

	angle2 = angle2 + (dt * 1000)
	angle3 = angle3 + (dt * 1500)

	return bullets, #bullets
end
