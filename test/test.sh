#!/bin/bash
#До скольки потоков выполнять тест
THR_NUM=$1
#Количество операций
ITER_NUM=$2
#Количество тестов
LIMIT=$3
#Выбор алгоритма транзакций
ITM_METHOD=$4
#Количество операций на поток
SIZE_THREAD=$(echo "scale=0; $ITER_NUM/$THR_NUM" | bc)

#echo "До $THR_NUM потоков"
#echo "Количество операций на все потоки: $ITER_NUM"
#echo "Количество тестов: $LIMIT"

PROG=main-tm

#Выбор метода
case $ITM_METHOD in
	
	gl_wt)
		#echo "Метод: gl_wt"
		echo "Метод: gl_wt" > "$ITM_METHOD.txt"
		export ITM_DEFAULT_METHOD=gl_wt
	;;
	ml_wt)
		#echo "Метод: ml_wt"
		echo "Метод: ml_wt" > "$ITM_METHOD.txt"
		export ITM_DEFAULT_METHOD=ml_wt
	;;
	serial)
		#echo "Метод: serial"
		echo "Метод: serial" > "$ITM_METHOD.txt"
		export ITM_DEFAULT_METHOD=serial
	;;
	serialirr)
		#echo "Метод: serialirr"
		echo "Метод: serialirr" > "$ITM_METHOD.txt"
		export ITM_DEFAULT_METHOD=serialirr
	;;
	serialirr_onwrite)
		#echo "Метод: serialirr_onwrite"
		echo "Метод: serialirr_onwrite" > "$ITM_METHOD.txt"
		export ITM_DEFAULT_METHOD=serialirr_onwrite
	;;
	mutex)
		#echo "Метод: PThread mutex"
		echo "Метод: PThread mutex" > "$ITM_METHOD.txt"
		PROG=main-me
	;;
	spinlock)
		#echo "Метод: PThread spinlock"
		echo "Метод: PThread spinlock" > "$ITM_METHOD.txt"
		PROG=main-sl
	;;
	*)
		echo "Выбран неправельный метод!"
esac

#Запуск теста
var0=0
var1=1
while [ "$var1" -le "$THR_NUM" ]
do
	echo "Количество потоков: $var1" >> "$ITM_METHOD.txt"
	while [ "$var0" != "$LIMIT" ]
	do
		./$PROG $var1 $ITER_NUM >> "$ITM_METHOD.txt"
		var0=$(($var0+1))
	done
	var0=0
	var1=$(($var1+1))
done
