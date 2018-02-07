-- starts

local filemask = "img%04d.tga"
local pool = 4 -- working threads pool size
local nx, ny = 1600, 1200 -- image dimension
local sensitivity = 0.02

-- xn+1 = sin(a yn) + c cos(a xn)
-- yn+1 = sin(b xn) + d cos(b yn)

local geomlst = {
--	{mina,    maxa,     minb,     maxb,     minc,     maxc,     mind,     maxd      },
	{ 1.6,     1.3,     -0.6,      1.7,     -1.2,      0.5,      1.6,      1.4,     },
	{ 0.6,     1.2,      1.2,      1.8,     -0.5,      1.8,      1.6,      1.4,     },
	{ 0.25,    1.75,    -1.12,     1.6,     -0.4,      1.8,     -1.8,      0.4,     },
	{ 1.6,     1.6,     -0.8,     -0.4,     -1.3,     -1.1,      1.5,      1.7,     },
}
local g = clifford.new() -- make clifford geometry structure
g.frames, g.iters, g.skip = 10000, 10000, 10 -- iterations parameters
g.minx, g.maxx = -4.0, 4.0
g.miny = g.minx * ny / nx
g.maxy = g.maxx * ny / nx

local img = image.create(nx, ny)
for i, p in ipairs(geomlst) do
	print(string.format("a[%g,%g] b[%g,%g] c[%g,%g] d[%g,%g]",
		p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]))
	g.mina, g.maxa = math.acos(p[1] / 2.0), math.acos(p[2] / 2.0)
	g.minb, g.maxb = math.acos(p[3] / 2.0), math.acos(p[4] / 2.0)
	g.minc, g.maxc = math.acos(p[5] / 2.0), math.acos(p[6] / 2.0)
	g.mind, g.maxd = math.acos(p[7] / 2.0), math.acos(p[8] / 2.0)

	img:clear()
	local dur = g:render(pool, img, "hue") -- call multithreaded render
	print("\r"..dur.."s            ")

	local filename = string.format(filemask, i)
	io.write("writing...")
	img:writetga(filename, sensitivity)
	print("\r"..filename.."      ")
end

-- the end.
