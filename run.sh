cd build

./BenchmarkVectorSwap --benchmark_out=results_vector_swap.json --benchmark_out_format=json
./BenchmarkVectorEraseIf --benchmark_out=results_vector_erase_if.json --benchmark_out_format=json

./BenchmarkMap --benchmark_out=results_map.json --benchmark_out_format=json
./BenchmarkUnorderedMap --benchmark_out=results_unordered_map.json --benchmark_out_format=json