set -x
./aircon_controller 172.17.0.1:5683 github 0d7bfea9a565390cd7a8 0   2>&1 | tee "$0.log"
