#Shouldn't need to run this but included for completeness
POLLY_VERSION="0.10.4"
wget "https://github.com/ruslo/polly/archive/v${POLLY_VERSION}.tar.gz"
tar xf "v${POLLY_VERSION}.tar.gz"
export POLLY_ROOT="`pwd`/polly-${POLLY_VERSION}"
export PATH="${POLLY_ROOT}/bin:${PATH}" # If you want to use build.py script