#! /bin/sh -e

export PKG_CONFIG_PATH="$HOME/brew//opt/openssl/lib/pkgconfig:$PKG_CONFIG_PATH"
make "$@"
