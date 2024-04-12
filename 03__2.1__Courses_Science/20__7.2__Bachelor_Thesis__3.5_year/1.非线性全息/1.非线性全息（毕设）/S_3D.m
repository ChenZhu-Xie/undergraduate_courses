clc;clear;
T = 25;
q = 9;%y����ʾʱ��������
%��������Ϊy���򣬾������Ϊz��,��Ƶ

Tx = 4; Tz = 4; Ty = 3;   %ʵ�ռ����سߴ�
%���� 
nx = 2; ny = 1; nz = 0; %����ʸ����
Gx = nx*2*pi/Tx; Gy = ny*2*pi/Ty; Gz = nz*2*pi/Tz;  %�������뵹��ʸ

%����ƥ�䲨��-ϸ�ڵ���ʸ����
set_lam1 = 0.5:0.001:1.5;
set_lam2 = set_lam1/2;
set_k1 = C_n(set_lam1,T)*2*pi./set_lam1;
set_k2 = C_n(set_lam2,T)*2*pi./set_lam2;
set_gy = sqrt(set_k2.^2-Gx^2)-2*set_k1-Gy;
% figure();plot(set_gy,set_lam1);

%�ṹ��λ�ֲ��Ż�
mx = 16; mz = 16; my = 16;  %ʵ�ռ����ظ���
x = Tx*(-mx/2:mx/2-1);  %ʵ�ռ�����x
y = Ty*(-my/2:my/2-1);  %ʵ�ռ�����y
z = Tz*(-mz/2:mz/2-1);  %ʵ�ռ�����z
[X,Y,Z] = meshgrid(x,y,z);
r = sqrt(X.^2+Y.^2+Z.^2);

kx = 2*pi/(Tx*(mx))*(-mx/2:mx/2-1);   %���ռ�����kx
ky = 2*pi/(Ty*(my))*(-my/2:my/2-1);   %���ռ�����ky
kz = 2*pi/(Tz*(mz))*(-mz/2:mz/2-1);   %���ռ�����kz
[KX,KY,KZ] = meshgrid(kx,ky,kz);
KR = sqrt(KX.^2+ KY.^2 + KZ.^2);

set1 = 9; %����ƥ�䵹��ʸλ��
disp(['����ϸ�ڵ���ʸ��' num2str(ky(set1))]);
index1 = find(abs(set_gy-ky(set1))==min(abs(set_gy-ky(set1))));
disp(['ƥ�䲨����' num2str(set_lam1(index1))]);

%������y������,���㲨ʸ
lam1 = set_lam1(index1);
lam2 = lam1/2;
n1 = C_n(lam1,T);
n2 = C_n(lam2,T);
k1 = n1*2*pi/lam1;
k2 = n2*2*pi/lam2;

%��ʼ��Ŀ��
SX = -Gx; %��������
SY = -(Gy + 2*k1);
SZ = -Gz;
SR = k2;
[xsurf,zsurf] = meshgrid(kx,kz);
ysurf2 = SR^2 - (xsurf-SX).^2 - (zsurf-SZ).^2; 
ysurf = sqrt(ysurf2.*(ysurf2>=0))+SY;  %�������λƥ�����棬��һ����Ҫ�ҳ���ɢ����ʸ�е����λ��

aim1 = im2double(rgb2gray(imread('xiaolian3.jpg')));  %��ʼ��Ŀ�곡�ֲ�
aim1 = flipud(sqrt(aim1));aim1 = aim1./max(max(aim1));
aim_F = zeros(my,mx,mz);  %��ʼ������ʸ�ֲ�����Ӧ������(ky,kx,kz)
[Mx,Mz] = meshgrid(1:mx,1:mz);

for ii = 1:mx
    for jj = 1:mz
        [~,index_ysurf] = sort(abs(ysurf(ii,jj)-ky));
        aim_F(index_ysurf(1),Mx(ii,jj),Mz(ii,jj)) = aim1(ii,jj);
    end
end
O_C = ones(my,mx,mz);
aim_F = aim_F./max(max(max(abs(aim_F))));
aim_F = 1*(aim_F>0.5)+0.05*(aim_F<=0.5);

%��ͼ
% figure();slice(KX,q*KY,KZ,aim_F,[],q*ky,[]);
% shading flat;
% axis equal;xlabel('KX');ylabel('KY');zlabel('KZ');title(['�Ż�Ŀ��--��������:' num2str(lam1)]);
% axis([min(kx),max(kx),min(q*ky),max(q*ky),min(kz),max(kz)]);
figure();set(gcf,'position',[40,0,600,600]);
for i = 1:my
subplot(4,4,i);imagesc(rot90(squeeze(aim_F(i,:,:)),1));title(['aim-' num2str(i)]);axis off;caxis([0,1]);
end

%��������Ҷ�Ż�
F = aim_F;
for i = 1:1e4
C = 1/(mx*my*mz)*fftshift(ifftn(fftshift(F)));
C = O_C.*exp(1i*angle(C));
F = 1/(mx*my*mz)*fftshift(fftn(fftshift(C)));
F = aim_F.*exp(1i*angle(F));
end
H = (angle(C));  %�Ż���ʵ�ռ���λ�ֲ�


%��ͼ
% figure();slice(X,40*Y,Z,H,[],40*y,[]);
% shading flat;axis equal;
% figure();set(gcf,'position',[40,60,900,500]);
% for i = 1:my
% subplot(3,5,i);imagesc(rot90(squeeze(H(i,:,:)),1));title(['H-' num2str(i)]);axis off;caxis([-pi,pi]);
% end

e = 0;
HH = zeros(my+e,mz,mx);
HH(e/2+1:my+e/2,:,:) = H;
%��֤
F = fftshift(fftn(fftshift(exp(1i*HH))));
F = F./max(max(max(abs(F))));
I_F = abs(F).^2; 
P_F = angle(F);

%��ʾ
%{
figure();
for num = 1:my
xslice = []; yslice = ky(num); zslice = [];
slice(KX,q*KY,KZ,I_F,[],q*yslice,[]);title([num2str(num),num2str(squeeze(max(max(I_F(num,:,:)))/1e5))]);
xlabel('KX');ylabel('KY');zlabel('KZ');
shading flat;
axis equal;axis([min(kx),max(kx),min(q*ky),max(q*ky),min(kz),max(kz)]);
pause(0.5);
end


[Mx,My,Mz]= meshgrid(1:mx,1:(my+e),1:mz);
figure();slice(Mx,(mx/my)*q*My,Mz,I_F,[],(mx/my)*q*(1:my+e),[]);
shading flat;
axis equal;xlabel('KX');ylabel('KY');zlabel('KZ');title(['�Ż����--��������:' num2str(lam1)]);
% axis([min(kx),max(kx),min(q*ky),max(q*ky),min(kz),max(kz)]);
[Mx,My,Mz]= meshgrid(1:mx,1:(my+e),1:mz);
figure();slice(Mx,(mx/my)*q*My,Mz,P_F,[],(mx/my)*q*(1:my+e),[]);
shading flat;
axis equal;xlabel('KX');ylabel('KY');zlabel('KZ');title(['�Ż����--��������:' num2str(lam1)]);
%}


figure();set(gcf,'position',[40,0,600,600]);
for i = 1:my
subplot(4,4,i);imagesc(rot90(squeeze(I_F(i,:,:)),1));title(['I-' num2str(i)]);axis off;caxis([0,1]);
end

% figure();set(gcf,'position',[40,60,900,500]);
% for i = 1:my
% subplot(3,5,i);imagesc(rot90(squeeze(P_F(i,:,:)),1));title(['P-' num2str(i)]);axis off;caxis([-pi,pi]);
% end
save H H;