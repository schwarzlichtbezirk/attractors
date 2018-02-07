-- starts

local filename = "buddhabrot.tga"
local pool = 4 -- working threads pool size
local nx, ny = 1000, 1000 -- image dimension
local sensitivity = 0.02

local g = buddhabrot.new() -- make buddhabrot geometry structure
g.nmax = 200 -- length of sequence to test escape status, also known as bailout
g.tmax = 50 -- number of iterations, multiples of 1 million

local img = image.create(nx, ny)
img:clear()
local dur, nsum, numdiscard = g:render(pool, img, "rainbow") -- call multithreaded render
print("\r"..dur.."s            ")

print("discard points: "..(100 * numdiscard / g.frames).."%")
print("average bailout: "..(nsum / (g.frames - numdiscard)))
print("total iterations: "..(nsum / 1e6).."M")

io.write("writing...")
img:writetga(filename, sensitivity)
print("\r"..filename.."      ")

-- the end.
