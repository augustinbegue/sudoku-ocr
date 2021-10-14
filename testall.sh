if make ; then
    echo ""
    echo "=== 1. TESTING PROCESSING ==="
    for i in $(seq 1 6);
    do
        echo ""
        echo "Processing image n.$i"
        rm -rf ./assets/output/output_$i 
        if ./main ./assets/grids/image_0$i.jpeg -o ./assets/output/output_$i.png -v ./assets/output/output_$i; then echo ""
        else break
        fi
        
    done

    rm -rf ./assets/output/output_5_rotated
    echo "=== 2. TESTING ROTATION ==="
    ./main ./assets/grids/image_05.jpeg -o ./assets/output/output_5_rotated.png -v ./assets/output/output_5_rotated -r 35

fi