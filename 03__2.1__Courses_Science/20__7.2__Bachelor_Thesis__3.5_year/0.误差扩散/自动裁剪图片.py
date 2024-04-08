# -*- coding: utf-8 -*- #加了这一行，也无法改变opencv库的cv2.imread无法读取中文路径的现实，包括文件名都不能是中文的！

import cv2
import numpy as np
from matplotlib import pyplot as plt

img = []
for i in range(7):
    img.append(cv2.imread("D:\\Users\\ZML\\Desktop\\%s.jpg" % (i+1), 0)) # 按绝对路径读取图片
# # 2号参数1 cv2.IMREAD_UNCHANGED=-1 (读取每点的BGR的同时，也考虑每点的C地，生成原图对象)
# # 2号参数2 cv2.IMREAD_GRAYSCALE=0 (只读取C而不读取每点的BGR地，生成灰度图对象)
# # 2号参数3 cv2.IMREAD_COLOR=1 (读取每点的BGR但不读取C地，生成彩色图对象)
# # 2号参数4 cv2.COLOR_BGR2RGB (似乎包括cv2.COLOR_BGR2GRAY等的所有通道转换都没用！！虽不报错但没用！)
# ----------------------------------- 图片属性
# print(img.shape) # 返回一个tuple = (行数,列数,通道数) 获取图像形状
# print(img.size) # 返回一个int = 行数 × 列数 × 通道数， 获取图像尺寸
# print(img.dtype) # 获取图像/对象的数据类型
# ----------------------------------- 单像素读取和修改【注意图片左上角像素点坐标为(0, 0)】
# print(img[88, 142]) # 像素读取：返回一个list = [B,G,R]，读取某个像素点所对应的三通道的值
# img[88, 142] = 255 # 像素修改1：给该像素点(x,y)的BGR三通道都赋值255，即变成白色
# img[88, 142] = [0, 0, 0] # 像素修改2：像素点相应颜色设为黑色
# img[88, 142, 0] = 1 # 像素修改3：单独修改B通道
# img[88, 142, 1] = 2 # 像素修改3：单独修改G通道
# img[88, 142, 2] = 3 # 像素修改3：单独修改R通道
# print(img[88, 142]) # 调试查看是否修改成功
# ----------------------------------- 区域像素读取和修改
# img[50:100, 100:200] = [255, 255, 255] # 切片：取img对象在x方向的序号为(50, 99)的[第51行,第100行]共50行、y方向的[第101行,第200行]共100行，间所有像素点，的BGR属性，以循环的形式，挨个被赋值
# ----------------------------------- 获取感兴趣的区域 ROI（Region of Interest）：将选定区域在另一窗口打开
img_RIO = []
for i in range(7):
    img_RIO.append(np.ones((1497-148, 1035-238, 3))) # numpy.ones = ones(shape, dtype=None, order='C') 定义200*100矩阵，3对应BGR三个通道：numpy中的ones输出的也是BGR？！
    img_RIO[i] = img[i][148:1497, 238:1035] # 将img的部分区域的对象的内容，copy到新创建的img_RIO对象的内容中去
# ----------------------------------- 图像覆盖：复制粘贴大法
# img[0:50, 0:100] = img[50:100, 100:200] # 单个图片内复制粘贴覆盖
# img[0:50, 0:100] = img_RIO[0:50, 0:100] # 多个图片的各部分相互覆盖
# ----------------------------------- 图像加法运算：目标图像 = (图像1 + 图像2)%255 或 目标图像 = 图像1 + 图像2
# sum1 = img + img # 方法一：Numpy加法运算：之和>255时取模，也称取模运算；————两对象必须尺寸相同，相同坐标的像素点的BGR通道各自对应相加，如img的B值与img_RIO的B值相加，返回sum1对象的相应坐标的像素点的B值
# sum2 = cv2.add(img, img) # 方法二：OpenCV加法运算；之和>255时取255，也称饱和运算；
# ----------------------------------- 图像融合（Never look away）：目标图像 = 图像1 * 系数1 + 图像2 * 系数2 + 亮度调节量
# ronghe = cv2.addWeighted(img, 1, img, 1, 0) # 与OpenCV加法运算类似 ，也是饱和运算：dst = cv2.addWeighter(scr1, alpha, src2, beta, gamma) 的意思就是 dst = src1 * alpha + src2 * beta + gamma，其中gamma不是默认参数不能省
# ----------------------------------- 通道拆分
# b = img[:, :, 0] # 拆分方式1：将原对象img的每个像素点的B值，覆盖掉其G、R值，生成一个新灰度图对象，并返回
# g = img[:, :, 1] # 拆分方式1
# r = img[:, :, 2] # 拆分方式1
# b, g, r = cv2.split(img) # 拆分方式2：等价于 b = cv2.split(a)[0] g = cv2.split(a)[1] r = cv2.split(a)[2]
# ----------------------------------- 单通道置零
# rows, cols, chn = img.shape
# b = cv2.split(img)[0] # 只显示蓝色：只提取B颜色通道，G、B通道灰度图各像素点的灰度值设置为0
# g = np.zeros((rows, cols), dtype=img.dtype) # zeros(shape, dtype=float, order='C')
# r = np.zeros((rows, cols), dtype=img.dtype) # dtype必须传入，否则默认float
# ----------------------------------- 通道合并
# M = cv2.merge([b, g, r]) # 按照B,G,R的顺序接收传入的通道参数/灰度图对象，所以不要 cv2.merge([r, g, b])；传入的各通道对象，其内容的矩阵尺寸必须相同
# ----------------------------------- 通道转换
# ctransform = cv2.cvtColor(img, cv2.COLOR_BGR2RGB) # 效果与 通道BGR拆分+通道RGB合并 一样
# # 2号参数1 cv2.COLOR_BGR2GRAY # 常用的3种
# # 2号参数2 cv2.COLOR_BGR2RGB
# # 2号参数3 cv2.COLOR_GRAY2BGR
# # 九种常见的颜色空间：BGR、RGB、GRAY、HSV、YCrCb、HLS、XYZ、LAB、YUV，所有颜色空间两两间的排列有200多种
# ----------------------------------- 图像缩放
# dsize1 = cv2.resize(img, (160,160)) # cv2.resize(src, dsize[, result[. fx[, fy[, interpolation]]]])
# dsize2 = cv2.resize(img, None, fx=0.5, fy=0.5) # dsize表缩放倍数，fx和fy也可表示缩放倍数，dsize 或 fx和fy设置一个即可实现图像缩放
# rows, cols = img.shape[:2]
# dsize3 = cv2.resize(img, (int(cols*0.5), int(rows*0.5))) # 效果与 fx=0.5, fy=0.5 一样
# ----------------------------------- 图像旋转（绕z轴）
# rows, cols = img.shape[:2]
# M1 = cv2.getRotationMatrix2D((cols/2, rows/2), 30, 1) # 需算个旋转参数 cv2.getRotationMatrix2D(旋转中心&位似中心坐标, 逆时针旋转度数, 位似比) 后，再用二维旋转矩阵作用于数组而旋转图像
# rotated = cv2.warpAffine(img, M1, (cols, rows)) # cv2.warpAffine(原始图像, 旋转参数, 原始图像宽高)
# ----------------------------------- 图像翻转（绕x轴180°、绕y轴180°）
# flip1 = cv2.flip(img, 0) # flipCode = 0：绕x轴180°；cv2.flip(src, flipCode)
# flip2 = cv2.flip(img, 1) # flipCode > 0：则绕y轴180°；似乎“轴对称操作”一词更符合该函数的操作：图像翻转1次本身就把它背面朝观察者了...只有再加透视才能实现轴对称
# flip3 = cv2.flip(img, -1) # flipCode < 0：绕x轴180° 且 绕y轴180° 即 绕x=y轴180°
# ----------------------------------- 图像平移
# M2 = np.float32([[1, 0, 0], [0, 1, 100]]) # 需算个平移参数 np.float32([[1, 0, x], [0, 1, y]]) 后，再用二维平移矩阵作用于数组而平移图像
# shifted = cv2.warpAffine(img, M2, (img.shape[1], img.shape[0])) # cv2.warpAffine(image, M, (image.shape[1], image.shape[0]))
# ----------------------------------- 显示图片（暂停程序、等待按键，并显示图片）
for i in range(7):
    cv2.imshow("lena", img[i])  # cv2.imshow(窗口名, 图像名) 弹出窗口，命名窗口，并显示图片
    cv2.imshow("RIO%s" % (i+1), img_RIO[i])
# cv2.imshow("Sum1", sum1)
# cv2.imshow("Sum2", sum2)
# cv2.imshow("Ronghe", ronghe)
# cv2.imshow("B", b)
# cv2.imshow("G", g)
# cv2.imshow("R", r)
# cv2.imshow("Merge", M)
# cv2.imshow("Ctransform", ctransform)
# cv2.imshow("Dsize1", dsize1)
# cv2.imshow("Dsize2", dsize2)
# cv2.imshow("Dsize3", dsize3)
# cv2.imshow("Rotated", rotated)
# cv2.imshow("Flip1", flip1)
# cv2.imshow("Flip2", flip2)
# cv2.imshow("Flip3", flip3)
# cv2.imshow("Shifted", shifted)
cv2.waitKey(0) # 无限期等待键盘输入，若在此期间捕获到输入，则返回相应ASCII值，并函数调用结束，执行下一条语句
# cv2.destroyWindow('lena') # 不需要销毁窗口，因waitkey执行完毕后，脚本运行完毕，imshow函数也被迫运行结束，窗口自动关闭 | cv2.destroyAllWindows() 销毁所有窗口；cv2.destroyWindow(窗口名) 销毁相应窗口名的窗口
# ----------------------------------- 保持图片
for i in range(7):
    cv2.imwrite("D:\\Users\\ZML\\Desktop\\RIO_%s.jpg" % (i+1), img_RIO[i]) # cv2.imwrite(文件名, 图片对象) 将图片从内存写入硬盘，默认目录为该.py文件所在的当前目录D:\Users\ZML\Desktop\test\Pycharm\