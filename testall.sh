if make ; then
    for i in {1..6}
    do
        echo ""
        echo "Processing image n.$i"
        if ./main ./assets/grids/image_0$i.jpeg -o ./assets/output_$i.bmp -m ./assets/output_$i.gr.bmp; then echo ""
        else break
        fi
        
    done
fi