function [c,C,A0,a0,h0,w0,d] = detour(Io,phase,level,A,a,h,w,l) 
% detour %(�ӹ��ṹ��Χ��������λ�ֲ�����λ�ּ����ṹ�ߴ硢��Ԫ�ߴ硢С�׸߶ȡ�С�׿�ȡ�Ŀ�����伶)
% ��������λ�ֲ�ת��Ϊ�ػ���λ�ṹ(���������)
Maxshift = a/l;  %���ƫ������um��
phase = mod(phase,2*pi);
d = Maxshift/(level-1);
shift = round(phase./(2*pi)*(level-1));
[M,~] = size(shift);
%����С�ס��ػ���λ��Ԫ������ṹ����ά��
mA = A/d + 1;
ma = a/d;
mh = round(h/d);
mw = round(w/d);
mC = M*ma+1;
C = zeros(mA);
c = zeros(M);
x0 = round((mA-mC)/2)-(ma-1);  y0 = round((mA-mC)/2)-(ma-1)+round((ma-mh)/2);
pos_x = x0 + cumsum(ma*ones(M),2)+shift;
pos_y = y0 + cumsum(ma*ones(M),1);    
[index_x,index_y] = find(Io == 1);
for num = 1:length(index_x)
    i = index_x(num);j = index_y(num);
    if shift(i,j)+mw < ma
        C(pos_y(i,j):pos_y(i,j)+mh-1,pos_x(i,j):pos_x(i,j)+mw-1) = 1;
    else
        C(pos_y(i,j):pos_y(i,j)+mh-1,[pos_x(i,j)-shift(i,j):pos_x(i,j)+mw-ma pos_x(i,j):pos_x(i,j)-shift(i,j)+ma-1]) = 1;
    end
    c(i,j) = (j-1)*a + d*shift(i,j);
end
A0 = d*(mA-1);a0 = d*(ma-1);h0 = d*(mh-1);w0 = d*(mw-1);
%���CΪ�ṹ�ֲ���cΪ�ṹ����
%A0\a0\h0\w0,���ذ�����ɢ������ʵ�ߴ�
end

