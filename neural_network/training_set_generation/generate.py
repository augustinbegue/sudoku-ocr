from PIL import Image, ImageFont, ImageDraw

size = (28, 28)

number = 1000
count = 0

img = Image.new('L', size, 255)
draw = ImageDraw.Draw(img)

font = ImageFont.truetype('arial.ttf', 28)

for digit in range(1, 9):
    draw.text((0, 0), str(digit), font=font, fill=0)
    img.save('training_set/' + str(digit) + '-{count:04}.png')
    count += 1
