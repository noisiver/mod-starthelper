#!/usr/bin/env bash
MOD_STARTHELPER_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $MOD_STARTHELPER_ROOT"/conf/conf.sh.dist"

if [ -f $MOD_STARTHELPER_ROOT"/conf/conf.sh" ]; then
    source $MOD_STARTHELPER_ROOT"/conf/conf.sh"
fi
