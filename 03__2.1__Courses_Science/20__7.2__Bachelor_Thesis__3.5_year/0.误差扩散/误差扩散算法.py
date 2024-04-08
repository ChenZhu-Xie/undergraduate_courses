# -*- coding:utf-8 -*-

import cv2
import numpy as np
from matplotlib import pyplot as plt
import random
import math

def E(i, j): # 函数里用到的同名变量，应该指向的是全局变量的内容/值。只不过python它弱化了全局变量和局部变量的说法= =，只要在函数外面定义了，就默认为是全局变量了
    return H_new_function(i, j) - H_new_function(i, j) / abs(H_new_function(i, j)) # 注意，这里不将 H_new_function(i, j) / abs(H_new_function(i, j)) 赋值给 H_p[i, j]，即暂时不慌 H_p[i, j] = H_new_function(i, j) / abs(H_new_function(i, j))
def H_new_function(i, j): # 为了沿用旧法来调用H_new，这样H_new_function(0,0) 就对应 H_new[1,1]；注意，函数名和变量名不能冲突，因为函数也是个变量！所以这里的函数名不能取H_new，因为在该函数被调用之前，H_new已经被定义为变量了；同理，由于上面已经有E(i,j)了，所以再也不能定义以E为名的变量了，如二维数组E[i,j]（这个倒没什么影响）。
    return H_new[i+1, j+1]
# ——————————————————————————————————— 加载原图并显示之
img = cv2.imread(r'D:\Users\ZML\Desktop\test.png', 0) # 按绝对路径读取图片
# cv2.imshow("lena", img) # cv2.imshow(窗口名, 图像名) 弹出窗口，命名窗口，并显示图片 现在直接全用大图来显示各个小图了
plt.figure(figsize=(7, 12), dpi=300) # 图中图的大底板图，长=3英寸，宽=6英寸，每英寸300像素，共1200*2700像素
# plt.subplots_adjust(hspace=0.3, wspace=0.3)
plt.subplot(621), plt.imshow(img, 'gray'), plt.title('img', fontsize = 10) # 在大图上显示第一个小图
# ----------------------------------- 二维快速傅里叶变换 和 逆变换
H_img = np.fft.ifft2(img)
H_p_angle__img = np.angle(H_img) # H_angle__img 的表达方式与 H_p_angle__img 是一样的，正如 H_p__img 的表达方式与 H_angle_p__img 一样，一样。
H_p__img = np.zeros((H_img.shape[0], H_img.shape[1]), dtype=complex) # 之后要让其储存复数值，所以这里得先给它定义一下类型，不然储存不了复数值，这与接下来的img_complex = np.fft.ifft2(img)其实一样
for i in range(H_p_angle__img.shape[0]):
    for j in range(H_p_angle__img.shape[1]):
        # print(H_img_angle[i, j])
        H_p__img[i, j] = complex(math.cos(H_p_angle__img[i, j]), math.sin(H_p_angle__img[i, j])) # 也只是暂时储存值，将angle相位（是纯角度，是实数）转换为相位部分，即虚数。若sin前是负，则最后傅里叶逆变换回去的重现像是倒的。
reconstructedimg__H_p__img = np.abs(np.fft.ifft2(H_p__img)) # 真正的傅里叶逆变换
# ----------------------------------- 显示不贴随机相位片的原img的 傅里叶变换后的 只取相位部分的全息图 H_p__img 的 重建像 reconstructedimg__H_p__img
plt.subplot(623), plt.imshow(H_p_angle__img, 'gray'), plt.title('H_p_angle__img', fontsize = 10) # 无法显示 H_p__img，因为是复数数组，所以只能委曲求全= =
plt.subplot(624), plt.imshow(reconstructedimg__H_p__img, 'gray'), plt.title('reconstructedimg__H_p__img', fontsize = 10)
# ——————————————————————————————————— 给原图贴上随机相位片，贴完后是复数数组，不能显示！随机相位片若不能转换为振幅，也不能显示！不知道那matlab教材里为什么可以显示，应该是转换为了振幅。
img_complex = np.fft.ifft2(img) # 只是先定义一个与img相同的可包含虚数的数组，赋什么值不重要（img只能含实数，导致对其的赋值如果包含虚数，会丢掉虚数，只把实数部分赋值给img）
for i in range(img.shape[0]):
    for j in range(img.shape[1]):
        randnum = random.uniform(0, 2*math.pi)
        img_complex[i, j] = img[i, j] * complex(math.cos(randnum), math.sin(randnum)) # 给物前加个随机相位片
img_random = img_complex
# -----------------------------------
plt.subplot(622), plt.imshow(np.angle(img_random), 'gray'), plt.title('img_random_angle', fontsize = 10)
# ----------------------------------- 二维快速傅里叶变换 和 逆变换
H = np.fft.fft2(img_random) # 二维快速傅里叶变换算法得到频率分布，返回一个复数数组；fft2(a, s=None, axes=(-2, -1), norm=None)
# Hcopy = np.fft.fft2(img_random) # 不能写 Hcopy = H，因为这样 Hcopy 与 H 都指向同一块区域和内容
H_p_angle = np.angle(H) # 直接取相位和接下来直接取相位部分
H_p = np.zeros((H.shape[0], H.shape[1]), dtype=complex) # 之后要让其储存复数值，所以这里得先给它定义一下类型，不然储存不了复数值，这与img_complex = np.fft.ifft2(img)其实一样
for i in range(H_p_angle.shape[0]):
    for j in range(H_p_angle.shape[1]):
        # print(H_angle[i, j])
        H_p[i, j] = complex(math.cos(H_p_angle[i, j]), math.sin(H_p_angle[i, j])) # 也只是暂时储存值，将angle相位（是纯角度，是实数）转换为相位部分，即虚数。
reconstructedimg__H_p = np.abs(np.fft.ifft2(H_p)) # 真正的傅里叶逆变换
# ----------------------------------- 显示贴了随机相位片后的img的 傅里叶变换后的 只取相位部分的全息图 H_p 的 重建像 reconstructedimg__H_p
plt.subplot(625), plt.imshow(H_p_angle, 'gray'), plt.title('H_p_angle__img_random', fontsize = 10) # 无法显示H_p或者说H_p__img_random，因为是复数数组，所以只能委曲求全= =
plt.subplot(626), plt.imshow(reconstructedimg__H_p, 'gray'), plt.title('reconstructedimg__H_p__img_random', fontsize = 10)
# ——————————————————————————————————— 对二维数组内，以（比二维低一维的）一维数组为单位，进行访问or查看的基本操作：二维切片
# print(H.shape) # 查看一下 原复振幅全息图 这个二维数组的尺寸；尺寸显示为：(m行, n列)，看样子该 shape函数 输出的应该是个tuple
# print(H.shape[0]) # shape函数输出的是个tuple，所以可以以这种方式查看其中元素
# print(H[0, :],H[0, :].shape) # 查看一下 二维数组 第一行的元素，以及该一维数组的尺寸（横向尺寸即其像素宽度）；尺寸显示为：(n列,)，这是个单元素的tuple的典型长相【而不是(n列)】，更加印证了shape函数输出的是个tuple的观点
# print(H[-1, :],H[-1, :].shape) # 查看一下 最后一行元素，及其尺寸
# print(H[0:2, :], H[0:2, :].shape) # 查看第1~2行的元素（索引行号为[0, 2-1]），及其尺寸；尺寸显示为：(2行, n列)
# print(H[:, 0:2], H[:, 0:2].shape) # 查看第1~2列的元素（索引列号为[0, 2-1]），及其尺寸；尺寸显示为：(m行, 2列)
# ----------------------------------- 对每个数组内某块二维区域进行访问or查看：从两个维度分别进行一次（共两次），二维切片（并取交集）
# print(H[0:2, -2:-1], H[0:2, -2:-1].shape) # 右上角的 2行×1列 区域（注意是1列哦！且该列的序列号不是-1，而是-2，懂不起的话看python笔记！）
# ----------------------------------- 对每个数组内每个元素进行访问or查看：宽度为1的两个二维切片的交集
# ----------------------------------- 单向误差扩散
w = (7/16, 3/16, 5/16, 1/16) # 设定误差扩散系数；用list设定以便更改，毕竟是可变对象，tuple不好更改, w[0] 就是文献中的 w_1
# ----------------------------------- 最好不采用 列表生成式 以生成列表list，因为到最后还是要转换为array；更不应用生成器，有点小题大做，而且list之后若有需要也可以改成generator；
# H_p_ERD = [[0 for col in range(H.shape[1])] for row in range(H.shape[0])] # 创建一个内容可变、但尺寸不可变的对象（也就是不是生成器）：二维的list，定义该 准备接收误差扩散后的相位全息图 的 二维数组的 每一行的 宽度=列数=H.shape[1]=H的n列，并每一行的元素全都初始化赋值为0；接着再创造拥有同样内容的其他行，高度=行数=H.shape[0]=H的m行。
# print(np.array(H_p_ERD).shape) # list对象没法用shape查看其尺寸，只有将其转换为numpy库的array对象后，才能用numpy库的shape方法查看；list也没法用size方法查看，倒是len可以查看其一维长度。
# ----------------------------------- numpy库里有现成的生成array对象的方法：生成 任意维度 和 任意各维度尺寸 的 array 并将各元素初始化为0：np.zeros((3,5))
H_p_ERD = np.zeros((H.shape[0], H.shape[1]), dtype=complex) # dtype = complex 不能少；由于之后会单独对每个对象[i,j]进行赋值，所以需要提前生成对象；
# print(H_p_ERD.shape)
# H_new = np.zeros((H.shape[0]+1, H.shape[1]+2), dtype=complex) # 新建一个 H_new 以准备拓展它为 (m+1) × (n+2) 的矩阵；其实不需要定义其尺寸，和提前生成对象，之后赋值的时候会自定义其尺寸和指向；
# ----------------------------------- 扩充 m×n 的 H矩阵 为 (m+1) × (n+2) 的 H_new矩阵
# print(np.r_[H[-1, :], H[-1, 0], H[:, 0]]) # 注意，np.r_[,...,] 要求各矩阵（可>2个）的列数，即n值相等，但你可能会觉得，H[-1, :]是末行，按理说它的行数才与后两个单位矩阵，都为1啊！——在list看来是这样的，但要注意这里是array矩阵，其每一行或列，只要单独提取出来，都是一个列向量！！所以这里合并两个长度不同的一维数组，不能用行相等、合成后行数不变的行向合并np.c_[]，而要列相等、合成后列数不变的列向合并np.r_[]
# ↑↑↑这玩意很反人类，请注意！↑↑↑ 居然连行与列都能拼起来...这说明，任何一行或一列，单独提出来，都是个列向量...
# H_last_row_new = np.r_[H[-1, -1], H[-1, :], H[-1, 0]] # 将 最后一行的左右两列 分别加上 最后一行的 最后一个元素 和 第一个元素；注意 行式合并的np.c_[] 和 列式合并的np.r_[] 不是个函数！是什么CClass！所以没法用小括号，而要用中括号！
# ↓↓↓这玩意更反人类：由于H_last_row_new是一维数组，所以默认是列向量，为了能与 np.c_[H[:, -1], H, H[:, 0]] 列向相加，必须同列，那 H_last_row_new 必须转置！！！我是真的服了！！！
# H_new = np.r_[H_last_row_new.reshape((1,-1)), np.c_[H[:, -1], H, H[:, 0]]] # 将 H 与 H的第一列元素，H在左、其第一列元素在右 地，拼接成一个 m × (n+1) 的矩阵；然后将H的最后一列元素 放到 m × (n+1) 矩阵 前作为其 新的第一列元素，生成 m × (n+2) 矩阵；然后将 H_last_row_new 作为 m × (n+2) 矩阵的新首行，生成一个 (m+1) × (n+2) 矩阵。
# ----------------------------------- 扩充 m×n 的 H矩阵 为 (m+1) × (n+2) 的 H_new矩阵：第二种填充方法
H_last_row_new = np.r_[H[-1, -1], H[-1, :], H[-1, 0]]
# for i in range(H.shape[1]+2):
#     H_last_row_new[i] = H_last_row_new[i] / np.abs(H_last_row_new[i])
H_last_col_new = H[:, -1]
H_first_col_new = H[:, 0]
# for i in range(H.shape[0]):
#     H_last_col_new[i] = H_last_col_new[i] / np.abs(H_last_col_new[i])
#     H_first_col_new[i] = H_first_col_new[i] / np.abs(H_first_col_new[i])
H_new = np.r_[H_last_row_new.reshape((1, -1)), np.c_[H_last_col_new, H, H_first_col_new]]
# print(H_new_function(6, 152), H[6, 152], '|', H_new.shape, H.shape) # 看看新生成的 H_new 是不是如期生成了，与旧H对比一下看是不是想要的样子
# ----------------------------------- 对 H_new 的 m×n 的 部分进行误差扩散
# print(E(0, 0))
for i in range(H.shape[0]):
    for j in range(H.shape[1]):
        # print(i,j)
        H_new[i+1, j+1] = H_new_function(i, j) + w[0] * E(i, j - 1) + w[3] * E(i - 1, j - 1) + w[2] * E(i - 1, j) + w[1] * E(i - 1, j + 1) # 注意这里的E是双参数函数而不是二维数组，所以是小括号而不是中括号
# print(H_new) # 各元素：模不为 1 的复数；H_new是画不出来的：只有实振幅图，才能被plt画出来图，复数数组本身就是画不出来的，因为图=光强的分布，而光强是实数。
for i in range(H.shape[0]):
    for j in range(H.shape[1]):
        H_p_ERD[i, j] = H_new_function(i, j) / abs(H_new_function(i, j)) # 将第i+1行第j+1列的 H_p 的 相位部分（非angle） 置为 H_new 的 相应i+2行 & j+2列的 相位部分，并将其模归一；其中/为浮点除法，所以可以保证归一化；
# print(H_p_ERD) # 各元素：模为 1 的复数；同理，H_p_ERD也是画不出来的，即使它的模归一化了，也是复数数组，即复振幅(全息)图。
# ----------------------------------- 二维快速傅里叶逆变换
H_p_ERD_angle = np.angle(H_p_ERD)
# print(H_p_ERD_angle) # 各元素：弧度为单位的 双精度小数 实数，在 - 3.14 ~ + 3.14 之间；按理说用plt绘图的时候，虽然这个是实数，因而可以绘图，但绘图出来应该振幅很小，以至于 灰度图 中 看不出来起伏，应该是一片黑or白才对————但好像会按百分比，将-3.14~+3.14 自动对应地转换为 0~255的灰度层级，所以看起来没问题。
H_p_ERD = np.zeros((H.shape[0], H.shape[1]), dtype=complex) # 之后要让其储存复数值，所以这里得先给它定义一下类型，不然储存不了复数值，这与img_complex = np.fft.ifft2(img)其实一样
for i in range(H_p_ERD_angle.shape[0]):
    for j in range(H_p_ERD_angle.shape[1]):
        # print(H_p_ERD_angle[i, j])
        H_p_ERD[i, j] = complex(math.cos(H_p_ERD_angle[i, j]), math.sin(H_p_ERD_angle[i, j]))
reconstructedimg__H_p_ERD = np.abs(np.fft.ifft2(H_p_ERD))
# ----------------------------------- 显示贴了随机相位片后的img的 傅里叶变换后的 误差扩散后的相位全息图H_p_ERD的 重建像 reconstructedimg__H_p_ERD
plt.subplot(627), plt.imshow(H_p_ERD_angle, 'gray'), plt.title('H_p_ERD_angle__img_random', fontsize = 10) # 无法显示 H_p_ERD 即 H_p_ERD__img_random，因为是复数数组，所以只能委曲求全= =
plt.subplot(628), plt.imshow(reconstructedimg__H_p_ERD, 'gray'), plt.title('reconstructedimg__H_p_ERD__img_random', fontsize = 10)
# ——————————————————————————————————— 对最后生成的复振幅重现像 np.fft.ifft2(H_p) 、 np.fft.ifft2(H_p_ERD) 进行误差扩散。。。emm，这不可能，因为最后取的是它的振幅，而不是它的相位。
# reconstructedimg_ERD__H_p = np.zeros((img.shape[0], img.shape[1]), dtype=complex)
# ——————————————————————————————————— GS算法：不贴随机相位的物像 img
def RMSE_functiton__img(i, j):
    return (abs(reconstructedimg__H_p__img[i, j]) - img[i, j])**2
RMSE = []
Number_of_iterations = 50
reconstructedimg_complex__H_p__img = np.zeros((img.shape[0], img.shape[1]), dtype=complex)
reconstructedimg_complex__H_p__img = img # 给complex矩阵赋值实矩阵没问题，并且不需要用for循环一个一个赋值
# -----------------------------------
for nth_interaction in range(Number_of_iterations):
    # - - - - - - - - - - - - - - - - - -
    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            reconstructedimg_complex__H_p__img[i,j] = reconstructedimg_complex__H_p__img[i,j] / abs(reconstructedimg_complex__H_p__img[i,j]) * img[i,j]
    # - - - - - - - - - - - - - - - - - -
    H_img = np.fft.ifft2(reconstructedimg_complex__H_p__img)
    H_p_angle__img = np.angle(H_img)
    H_p__img = np.zeros((H_img.shape[0], H_img.shape[1]), dtype=complex)
    for i in range(H_p__img.shape[0]):
        for j in range(H_p__img.shape[1]):
            H_p__img[i, j] = H_img[i, j] / abs(H_img[i, j])
    reconstructedimg_complex__H_p__img = np.fft.ifft2(H_p__img)
    reconstructedimg__H_p__img = np.abs(reconstructedimg_complex__H_p__img)
    # - - - - - - - - - - - - - - - - - -
    RMSE.append(0)
    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            RMSE[nth_interaction] = RMSE[nth_interaction] + RMSE_functiton__img(i, j)
    RMSE[nth_interaction] = (RMSE[nth_interaction] / img.shape[0] / img.shape[1])**0.5
# -----------------------------------
plt.subplot(629), plt.imshow(H_p_angle__img, 'gray'), plt.title('H_p_angle__img || after 50 iterations', fontsize = 10) # 无法显示 H_p__img，因为是复数数组，所以只能委曲求全= =
plt.subplot(6,2,10), plt.imshow(reconstructedimg__H_p__img, 'gray'), plt.title('reconstructedimg__H_p__img || after 50 iterations', fontsize = 10)
plt.subplot(616), plt.scatter(range(Number_of_iterations), RMSE, marker = '+',color = 'black', s = 10), plt.title('RMSE[nth_interaction]', fontsize = 10)
# ——————————————————————————————————— GS算法：贴随机相位的物像
# ——————————————————————————————————— 显示图片（暂停程序、等待按键，并显示图片）
# plt.tight_layout()
plt.show()
cv2.waitKey(0) # 无限期等待键盘输入，若在此期间捕获到输入，则返回相应ASCII值，并函数调用结束，执行下一条语句





# ————————————————————————————————————
# def E_O(i, j):
#     return O_new_function(i, j) - O_new_function(i, j) / abs(O_new_function(i, j))
# def O_new_function(i, j):
#     return O_new[i+1, j+1]
# -----------------------------------
# O = img # 根据 下下行的代码的注释 O_img 不该是 O 的简单取相位，而是 O 与 参考光 一起近场菲涅尔干涉后，在像面形成的干涉图样，其中包含了参考光所引入的相位信息，也是个复振幅全息图后，再对其取相位；因此 O 便不能是简单的 img 而是 img 传播 并与参考光发生 叠加 后的 复振幅全息图！之后的O_new的生成和对O_new的扩散，也要求O_new是复振幅的全息图，因此要求O也是。——所以咱们就不要叫O了...还是H吧？...
# O_img = np.angle(O) # O是Original的意思（单独的 O = img，要 O_img 才是 POH 纯相位全息图），即近场菲涅尔衍射，在像面基本保留物平面的原图；这里也只截取振幅看看是不是也只有轮廓
# print(O_img) # 由于 O 原图像 只有 实振幅，所以其转换为 POH 时，全为实数零。————这样输出它的时候就会出问题；
# O_p = np.zeros((O.shape[0], O.shape[1]), dtype=complex)
# O_last_row_new = np.r_[O[-1, -1], O[-1, :], O[-1, 0]]
# O_last_col_new = O[:, -1]
# O_first_col_new = O[:, 0]
# O_new = np.r_[O_last_row_new.reshape((1, -1)), np.c_[O_last_col_new, O, O_first_col_new]]
# for i in range(O.shape[0]):
#     for j in range(O.shape[1]):
        # print(i,j)
#         O_new[i+1, j+1] = O_new_function(i, j) + w[0] * E_O(i, j - 1) + w[3] * E_O(i - 1, j - 1) + w[2] * E_O(i - 1, j) + w[1] * E_O(i - 1, j + 1)
# for i in range(O.shape[0]):
#     for j in range(O.shape[1]):
#         O_p[i, j] = O_new_function(i, j) / abs(O_new_function(i, j))
# O_p_img = np.angle(O_p)
# -----------------------------------
# plt.subplot(427), plt.imshow(O_img, 'gray'), plt.title('Fei Nie Er Image')
# plt.subplot(428), plt.imshow(O_p_img, 'gray'), plt.title('Fei Nie Er Image After ERD')
# ————————————————————————————————————