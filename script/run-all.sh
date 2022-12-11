RED='\033[0;31m'
NC='\033[0m'
echo -e "${RED}================================================================================"
echo -e "Running hoard allocator${NC}"
LD_PRELOAD=/usr/lib/libhoard.so bazel run -c opt $1
echo ""
echo -e "${RED}================================================================================"
echo -e "Running jemalloc allocator${NC}"
LD_PRELOAD=/usr/local/lib/libjemalloc.so bazel run -c opt $1
echo ""
echo -e "${RED}================================================================================"
echo -e "Running tcmalloc allocator${NC}"
LD_PRELOAD=/usr/lib64/libtcmalloc.so bazel run -c opt $1
echo ""
echo -e "${RED}================================================================================"
echo -e "Running ptmalloc allocator${NC}"
bazel run -c opt $1
echo -e "${RED}================================================================================${NC}"
