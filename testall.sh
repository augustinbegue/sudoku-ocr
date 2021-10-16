if make ; then
    # echo ""
    # echo "=== 1. TESTING PROCESSING ==="
    for i in $(seq 1 6);
    do
        echo ""
        echo "Processing image n.$i"
        if ./main ./assets/grids/image_0$i-big.jpeg -o ./assets/output/output_$i.png -v ./assets/output/output_$i; then echo ""
        else exit 1
        fi
        
    done

    # echo ""
    # echo "=== 2. TESTING ROTATION ==="
    # if ./main ./assets/grids/image_05.jpeg -o ./assets/output/output_5_rotated.png -v ./assets/output/output_5_rotated -r 35; then echo ""
    # else exit 2
    # fi

fi