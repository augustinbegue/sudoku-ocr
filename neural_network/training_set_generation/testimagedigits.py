import subprocess

# ⚠️ WARNING ⚠️ FOR THE SCRIPT TO WORK, YOU HAVE TO COPY THE SAVE FILE OF THE NEURAL NETWORK TO THE SAME DIRECTORY AS THIS SCRIPT

count = 308

for i in range(1, 6):
    for x in range(1, 9):
        for y in range(1, 9):
            process = subprocess.run(
                f"../../network test ../../assets/output/output_{i}/10-grid-{x}x{y}.png", shell=True)
            print(process.returncode)
            output = process.returncode

            if output >= 0 and output <= 9:
                count += 1
                subprocess.run(
                    f"cp ../../assets/output/output_{i}/10-grid-{x}x{y}.png ./training_set/{output}-{format(count, '04')}.png", shell=True)
