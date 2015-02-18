if type rlwrap  > /dev/null 2>&1; then
    exec rlwrap ./popo
else
    exec ./popo
fi
