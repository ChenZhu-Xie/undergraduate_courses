clc;clear;

%��ȡ�ṹ����
structure_path = 'Structure';  %�޸Ĳ�ͬ�ṹ��·��
load ([structure_path '\Structure_parameter.mat']);

%���ÿռ�����
x = linspace(-A/2,A/2,m);
y = x;
[X,Y] = meshgrid(x,y);
r = sqrt(X.^2+Y.^2);
fi = angle(X+1i*Y);
Z = linspace(0,L,n);

%����������������͹ⳡ�ֲ���ʵ���¶�
lam1 =0.850;lam2 = lam1;T = 25;
lam3 = 1/(1/lam1+1/lam2);
w0 = 80;
A1 = exp(-r.^2/w0^2); A2 = A1;
md = 0.15;  %�ӹ��������

%��ʼ����
[A3,profilex,profiley,d_k] = SSF_FD(A,L,m,n,lam1,lam2,T,A1,A2,structure_path,md);

%���ƽ��
%�۲������
M = 2*m;   %�۲�������
p = 1.2;  %�۲췶Χ
L_far = 50e3;  %Զ����L_far=�������룻͸������Ҷ�任��L_far= ���ࡣ

%�۲�������
fx = (-M/2+0.5:M/2-0.5).*(1/(M/m*A));
[Fx,Fy] = meshgrid(fx,fx);
x_far = fx*lam3*L_far;

A3 = padarray(A3,[(M-m)/2 (M-m)/2],0,'both');

% %{
%����Զ��
A3_far = (M*M)^(-1/2)*fftshift(fft2(fftshift(A3)));

%��ͼ
figure();
subplot(1,3,1);imagesc(x,x,abs(A3).^2);
subplot(1,3,2);imagesc(x_far,x_far,abs(A3_far).^2);title(num2str(lam1));
axis([-p*lam3*L_far,p*lam3*L_far,-p*lam3*L_far,p*lam3*L_far]);caxis([0,1e-6]);
% subplot(1,3,3);imagesc(x_far,x_far,angle(A3));
% axis([-p*lam3*L_far,p*lam3*L_far,-p*lam3*L_far,p*lam3*L_far]);
subplot(1,3,3);plot(x_far,abs(A3_far(fix(3*m/2+1),:)).^2);
axis([-p*lam3*L_far,p*lam3*L_far,0,1e-3]);
set(gcf,'position',[10,150,1400,300]);

figure();
subplot(2,2,1);imagesc(Z,x,abs(profilex).^2);title('x-profile-I');
subplot(2,2,2);imagesc(Z,x,angle(profilex));title('x-profile-Phase');
subplot(2,2,3);imagesc(Z,x,abs(profiley).^2);title('y-profile-I');
subplot(2,2,4);imagesc(Z,x,angle(profiley));title('y-profile-Phase');
set(gcf,'position',[10,50,800,500]);
%}
%Ч��
%{
px_target = 0;py_target = 0;
target = sqrt((Fx-px_target/Tx).^2+(Fy-py_target/Ty).^2)<(0.5/Tx);
E_in = sum(sum(abs(A1).^2))+sum(sum(abs(A2).^2));
E_out = sum(sum(abs(A3).^2));
E_far = sum(sum(abs(A3_far).^2));
E_target = sum(sum(abs(A3_far.*target).^2));
e_total = E_out/E_in;
e_target = E_target/E_in;

disp(['����������' num2str(E_in)]);
disp(['�������(����/Զ��)��' num2str(E_out) '/' num2str(E_far) '����Ч�ʣ�' num2str(e_total)]);
disp(['Զ��Ŀ��������' num2str(E_target) '��Ŀ��Ч�ʣ�' num2str(e_target)]);
%}