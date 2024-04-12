function [A3,profilex,profiley,d_k]= SSF_FD(A,L,m,n,lam1,lam2,T,A1,A2,structure_path,md)
%�ֲ�����Ҷ�㷨+���޲�ַ�
%�����������ⳡA3
kx = (-m/2+0.5:m/2-0.5).*(2*pi/A);    %����Ƶ������
ky = (-m/2+0.5:m/2-0.5).*(2*pi/A);
[Kx,Ky] = meshgrid(kx,ky);
d_step = L/n;    %����������

%����������λʧ����
lam3 = 1/(1/lam1+1/lam2);
k1 = C_n(lam1,T)*2*pi/lam1;
k2 = C_n(lam2,T)*2*pi/lam2;
k3 = C_n(lam3,T)*2*pi/lam3;
d_k = k1+k2-k3;
co = 4-2*(lam1==lam2);%���������ϵ��ϵ��

%һЩ��ʼ����
deff = 22e-6;c = 3e14;
w3 = 2*pi*c/lam3;

A3 = zeros(m);profilex = zeros(m,n);profiley = zeros(m,n);
H = fftshift(exp(-1i/(2*k3)*(Kx.^2+Ky.^2)*d_step));

%���ݷֲ�����Ҷ�����޲�ַ���������A3,ÿǰ��һ����ȡ��ǰ�ṹ����
for ii = 1:n
    load([structure_path '\Structure_face' num2str(ii) '.mat']);
    z = (ii-1)*d_step;
    d_A3_linear = ifft2(fft2(A3).*H);
    d_A3_nonlinear = (w3^2/(k3*c^2))*exp(1i*d_k*(z+d_step/2)).*((exp(1i*d_k*d_step/2)-1)/d_k).*A1.*A2.*(co*deff).*(1-md*face);
    A3 = d_A3_linear + d_A3_nonlinear;
    profilex(:,ii) = A3(fix(m/2+1),:);
    profiley(:,ii) = A3(:,fix(m/2+1));
    clc;disp(ii/n);
end
%�����������������A3��A3�ھ����в���������x����ͼ��A3�ھ����в���������y����ͼ
end

