#include<stdio.h>
#include<math.h>
#include<stdlib.h>
int j=0,count=0,*hang=(int*)malloc(sizeof(int)),mohang,*lie=(int*)malloc(sizeof(int));
int min(int m,int n)
{
	int t;
	t=m;
	if(n<m)
		t=n;
	return t;
}
void fun(int m,int n)
{
	int i,m1,n1,k;
	if(n>1)
	{
		for(i=0;i<=n-1;i++)
		{
			j++;
			n1=n-i;              /*valid*/
			hang=(int*)realloc(hang,j*sizeof(int));
			hang[j-1]=n1;
			m1=m-n1;             /*valid*/
			mohang=m1;
			n1=min(m1,n1);       /*valid*/
			fun(m1,n1);
			j--;
		}
	}
	else
	{
		count=count+1;
		if(n==1)
		{
			hang=(int*)realloc(hang,(j+mohang)*sizeof(int));
			for(i=j;i<=j+mohang-1;i++)
				hang[i]=1;
			/**/
			printf("按行从上到下计数:");
			for(i=j+mohang-1;i>=0;i--)
				printf("%d ",hang[i]);
			printf("\n");
			/**/
			lie=(int*)realloc(lie,(hang[0])*sizeof(int));
			for(k=0;k<hang[0];k++)
			{
				lie[k]=0;
				for(i=0;i<=j+mohang-1;i++)
				{
					if(hang[i]-k-1>=0)
						lie[k]=lie[k]+1;
				}
			}
			/**/
			printf("按列从左到右计数:");
			for(k=0;k<hang[0];k++)
				printf("%d ",lie[k]);
			printf("\n\n");
		}
		if(n==0)
		{
			printf("按行从上到下计数:");
			for(i=j-1;i>=0;i--)
				printf("%d ",hang[i]);
			printf("\n");
			/**/
			lie=(int*)realloc(lie,(hang[0])*sizeof(int));
			for(k=0;k<hang[0];k++)
			{
				lie[k]=0;
				for(i=0;i<=j-1;i++)
				{
					if(hang[i]-k-1>=0)
						lie[k]=lie[k]+1;
				}
			}
			/**/
			printf("按列从左到右计数:");
			for(k=0;k<hang[0];k++)
				printf("%d ",lie[k]);
			printf("\n\n");
		}
	}
}
void main()
{
	int m,n;
	printf("请像这样输入球的个数和筒宽,之间以逗号隔开'm,n':");
	scanf("%d,%d",&m,&n);
	fun(m,min(m,n));
	printf("\n如上所示,共有%d组解的组合。\n\n",count);
}
