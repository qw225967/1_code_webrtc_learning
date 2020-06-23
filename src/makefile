#第一版三要素，复用性不强，不能节省编译时间
app:remote_estimator_proxy.cc transport_feedback.cc main.cc createtestdata.cc
	g++ -std=c++11 -g timer.cc mymutex.cc remote_estimator_proxy.cc transport_feedback.cc main.cc createtestdata.cc -I../include -o app



#第二版,从顶向下分析建立依赖关系，自底向上执行命令 (通过第二版节省编译时间)

#app:add.o sub.o mul.o des.o main.o
#	gcc add.o sub.o mul.o des.o main.o -o app
#add.o:add.c
#	gcc add.c -c -I../include
#mul.o:mul.c
#	gcc mul.c -c -I../include
#des.o:des.c 
#	gcc des.c -c -I../include 
#sub.o:sub.c
#	gcc sub.c -c -I../include 
#main.o:main.c 
#	gcc main.c -c -I../include

#第三版，增加Makefile的复用性
#定义变量　变量没有类型（存储字符串数据）变量名用大写定义, 使用$(变量名)使用变量

#Makefile提供了一系列的函数，例如文件名处理函数和字符串处理函数


#####FILENAME=$(wildcard *.cc)
#####DESNAME=$(patsubst %.cc,%.o,$(FILENAME))

#通过Makefile内建语法完成.o文件的批处理

#Makefile特殊变量 $@　代表目标 $^ 代表所有依赖 $< 代表依赖中的第一个

#####app:$(DESNAME)
#####	g++ -std=c++11 $(DESNAME) -o app

#####%.o:%.c
#####	g++ -std=c++11 -I../include -c $<



######output:
######	echo $(FILENAME)
#####	echo $(DESNAME)

