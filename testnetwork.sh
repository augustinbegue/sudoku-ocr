if make; then
    echo "Test network"
    
    ./network train

    for i in {0..9}; do
        echo ""
        echo "EXPECTED $i"
        ./network test ./assets/training_set/$i-0285.png
    done
fi