clc;clear;
%����ά��λ�������ػ���λ����ķ�ʽת��Ϊ�ṹ����

%%%-------���ýṹ���ݴ洢·��----------%%%
structure_path = 'structure';
mkdir(structure_path);
rmdir(structure_path,'s');
mkdir(structure_path);

load H  %HΪ��ά��λ�ֲ�
[mh,nh,lh] = size(H);
I0 = ones(nh,lh);

%�ṹ����
L = 60;n = 600;Tz = 3;%�ṹ�������
A = 150;a = 4; 
h = 4;w = 1;
duty_cycle  = w/a;
l = 2;level = 30;

%��������
Z = linspace(0,L,n);

%�������ɽṹ
figure();
for i = 1:n
    z = Z(i);
    in = fix(z/Tz);
    if in <= mh && in>=1
        [c,C,A0,a0,h0,w0,d] = detour(I0,rot90(squeeze(H(in,:,:)),1),level,A,a,h,w,l);%�����ػ���λ�ṹ
    else
        C = zeros(A*l*(level-1)/a+1);
    end
    face = (mod(z,Tz)<=Tz/2).*C;%����GS��λ����ṹ
    save([structure_path '\Structure_face' num2str(i) '.mat'],'face');
    clc;disp([num2str(z) '/' num2str(L)]);
    imagesc(face);title(mod(z,Tz));pause(0.01);  %Ԥ��
end

[m,~] = size(face);
%�洢�ṹ����
save([structure_path '\Structure_parameter.mat'],'A','L','m','n');