main.cpp
从文件中读取 vessel 输入，输出桥吊作业计划到终端（也可自行改动代码输出到文件中），然后输出用于绘图的 bay_plan.txt，即以每个贝位为单位的桥吊作业计划，并现场画出桥吊作业计划图

vessel.h / vessel.cpp
定义了 Vessel 输入模型，以及实现了加载/保存的接口

plan.h / plan.cpp
定义了 CraneWorkingPlan 输出模型，以及实现了保存、转化为画图用的 bay_plan、合并移动动作、去除移动动作等接口

algo.h / algo.cpp
定义了 DPConfig 的算法参数，实现了状态压缩动态规划算法

draw.py
绘制桥吊作业计划图的脚本，输入是 bay_plan，用法 python draw.py BAY_PLAN_FILE_NAME

vessel_instances
|-	readme.txt
	vessel 文件格式说明文档
|-	ningbo.vessel / shanghai_1.vessel / shanghai_2.vessel
	测试用例