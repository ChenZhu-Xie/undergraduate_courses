clc;clear;
%将三维相位矩阵按照迂回相位编码的方式转换为结构数据

%%%-------设置结构数据存储路径----------%%%
structure_path = 'structure';
mkdir(structure_path);
rmdir(structure_path,'s');
mkdir(structure_path);

load H  %H为三维相位分布
[mh,nh,lh] = size(H);
I0 = ones(nh,lh);

%结构参数
L = 60;n = 600;Tz = 3;%结构纵向参数
A = 150;a = 4; 
h = 4;w = 1;
duty_cycle  = w/a;
l = 2;level = 30;

%建立坐标
Z = linspace(0,L,n);

%逐面生成结构
figure();
for i = 1:n
    z = Z(i);
    in = fix(z/Tz);
    if in <= mh && in>=1
        [c,C,A0,a0,h0,w0,d] = detour(I0,rot90(squeeze(H(in,:,:)),1),level,A,a,h,w,l);%生成迂回相位结构
    else
        C = zeros(A*l*(level-1)/a+1);
    end
    face = (mod(z,Tz)<=Tz/2).*C;%生成GS相位编码结构
    save([structure_path '\Structure_face' num2str(i) '.mat'],'face');
    clc;disp([num2str(z) '/' num2str(L)]);
    imagesc(face);title(mod(z,Tz));pause(0.01);  %预览
end

[m,~] = size(face);
%存储结构参数
save([structure_path '\Structure_parameter.mat'],'A','L','m','n');