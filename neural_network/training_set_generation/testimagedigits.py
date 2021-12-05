import os

count = [308] * 10

for i in range(1, 6):
    for x in range(1, 9):
        for y in range(1, 9):
            stream = os.popen(
                f"../../network test ../../assets/output/output_{i}/10-grid-{x}x{y}.png")
            output = stream.read()
            output = int(output.strip())

            if output >= 0 and output <= 9:
                count[output] += 1
                print(
                    f"cp ../../assets/output_{i}/10-grid-{x}x{y}.png ./training_set/{output}-{format(count[output], '04')}.png")
                # os.system(
                #     f"cp ../../assets/output_$i/10-grid -{x}x{y}.png ./training_set /{output}-{format(count[output], '04')}.png")
