cd build

./BenchmarkVectorSwap --benchmark_out=../reports/results_vector_swap.json --benchmark_out_format=json
./BenchmarkVectorEraseIf --benchmark_out=../reports/results_vector_erase_if.json --benchmark_out_format=json

./BenchmarkMap --benchmark_out=../reports/results_map.json --benchmark_out_format=json
./BenchmarkUnorderedMap --benchmark_out=../reports/results_unordered_map.json --benchmark_out_format=json

./BenchmarkSlotMap --benchmark_out=../reports/results_slot_map.json --benchmark_out_format=json