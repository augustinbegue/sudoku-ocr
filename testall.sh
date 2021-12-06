if make; then

    for i in $(seq 1 6);
    do
        echo ""
        echo "Processing image n.$i"
        if ./sudoku-ocr ./assets/grids/image_0$i.jpeg -o ./assets/output/output_$i.png -v ./assets/output/output_$i; then echo ""
        else exit 1
        fi
    done

fi