Программа для перемещения файлов в корзину, вместо их безвозвратного удаления

Использование:

make

LD_PRELOAD=./unlink.so rm <filepath>        // для вызова unlinkat(2)

LD_PRELOAD=./unlink.so unlink <filepath>    // для вызова unlink(2)