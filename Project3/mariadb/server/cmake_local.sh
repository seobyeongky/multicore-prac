#!/bin/bash
export MY_DB_PATH=$HOME/2016_ITE4065_2011003133/Project3/mariadb/run
export MY_DB_DOC_PATH=$HOME/2016_ITE4065_2011003133/Project3/mariadb/run/doc
export MY_DB_VAR_PATH=$HOME/2016_ITE4065_2011003133/Project3/mariadb/run/var
cmake \
-DCMAKE_INSTALL_PREFIX=$MY_DB_PATH \
-DINSTALL_SYSCONFDIR=$MY_DB_PATH/etc \
-DINSTALL_SYSCONF2DIR=$MY_DB_PATH/etc/my.cnf.d \
-DINSTALL_DOCDIR=$MY_DB_DOC_PATH \
-DINSTALL_DOCREADMEDIR=$MY_DB_DOC_PATH \
-DMYSQL_DATADIR=$MY_DB_PATH/data \
-DTMPDIR=$MY_DB_PATH/tmp \
-DMYSQL_UNIX_ADDR=$MY_DB_VAR_PATH/lib/mariadb/mariadb_$USER.sock \
-DPID_FILE_DIR=$MY_DB_VAR_PATH/run/mariadb_$USER \
-DDAEMON_NAME=mariadb_$USER \
-DWITHOUT_TOKUDB_STORAGE_ENGINE=ON \
-DWITHOUT_MROONGA_STORAGE_ENGINE=ON \
-DCMAKE_CXX_FLAGS=-DPROJECT3_PART1 \
-DCMAKE_CXX_FLAGS=-DPROJECT3_PART2 .

