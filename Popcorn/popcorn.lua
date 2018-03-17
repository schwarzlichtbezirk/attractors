-- starts

local filemask = "img%04d.tga"
local pool = 4 -- working threads pool size
local nx, ny = 1600, 1200 -- image dimension
local sensitivity = 0.02

-- xn+1 = xn - h sin(yn + tan(k yn))
-- yn+1 = yn - h sin(xn + tan(k xn))

local geomlst = {
--	{ h,       k,       },
	{ 0.05,    2.0,     },
	{ 0.05,    3.0,     },
	{ 0.05,    4.0,     },
	{ 0.05,    5.0,     },
}
local g = popcorn.new() -- make popcorn geometry structure
g.frames, g.iters = 10000, 20000 -- iterations parameters
g.minx, g.maxx = -8.0, 8.0
g.miny = g.minx * ny / nx
g.maxy = g.maxx * ny / nx

color.setpair(color.create(1.0, 0, 0), color.create(0, 0, 1.0))

local img = image.create(nx, ny)
for i, p in ipairs(geomlst) do
	print(string.format("h=%g k=%g", p[1], p[2]))
	g.h, g.k = p[1], p[2]

	img:clear()
	local dur = g:render(pool, img, "gradient") -- call multithreaded render
	print("\r"..dur.."s            ")

	local filename = string.format(filemask, i)
	io.write("writing...")
	img:writetga(filename, sensitivity)
	print("\r"..filename.."      ")
end

-- the end.
