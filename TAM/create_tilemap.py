from PIL import Image

out_image = Image.new("RGB", (4 * 256, 512))
x = 0
for tone in range(0,4):
    y = 0
    for mip in range(3,-1,-1):
        in_image = Image.open("default%d%d.bmp" % (tone, mip))
        in_width = in_image.size[0]
        in_height = in_image.size[1]
        out_image.paste(in_image, (x, y, x + in_width, y + in_height))
        y += in_height
    x += 256

out_image.save("mipped_hatches.png")
