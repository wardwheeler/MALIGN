/*Copyright 1995 Ward Wheeler all rights reserved*/
/*Make sure that the reconstructed alignments == the initial ones + can count gaps and changes from down pass as a check*/

#include "align3.h"
int get_jack_groups(ba,a,values)
alignment **a,**ba;
parameters *values;
{
int i,j,k,num_there=0;
int counter=0,ngroups,ntax;
int **groups;
int **big_groups, *num_of_each;
int same,counter2,counter3,in,l,cur_len;
int **final_groups,d1,d2,first;
char *group_names,*temp1;

groups=NULL;
big_groups=NULL;
num_of_each=NULL;
final_groups=NULL;
group_names=NULL;
temp1=NULL;

ntax=values->all_done;
big_groups=(int **)malloc(sizeof(int *));
assert((int) big_groups);
big_groups[0]=(int *)malloc((ntax+1)*sizeof(int));
assert((int) big_groups[0]);

for (i=0;i<values->jackboot;i++) {
	groups=get_groups4(values->jack_name[i],ntax,a,values,&ngroups);
	big_groups=(int **)realloc(big_groups,(counter+ngroups)*sizeof(int *));
	assert((int) big_groups);
	for (j=counter;j<counter+ngroups;j++) {
		big_groups[j]=(int *)malloc((ntax+1)*sizeof(int));
		assert((int) big_groups[j]);
		for (k=0;k<ntax+1;k++) big_groups[j][k]=groups[j-counter][k];
		free(groups[j-counter]);
		}
	free(groups);
	counter+=ngroups;
	}
fprintf(stderr,"There were a total of %d groups in %d trees\n",counter,values->jackboot);
num_of_each=(int *)malloc(counter*sizeof(int));
assert((int) num_of_each);
for (i=0;i<counter;i++) num_of_each[i]=1;
for (i=0;i<counter;i++) if (num_of_each[i]>0) {
	for (j=i+1;j<counter;j++) if (num_of_each[j]>0) {
		same=1;
		for (k=0;k<ntax+1;k++) if (big_groups[i][k]!=big_groups[j][k]) {same=0; break;}
		if (same) {
			num_of_each[i]++;
			num_of_each[j]=0;
			}
		}
	}
counter2=0;
for (i=0;i<counter;i++) {
	if (big_groups[i][ntax]==ntax) num_of_each[i]=0;
	if (num_of_each[i]>=((values->cutpoint*values->jackboot/100))) ++counter2;
	}
if (counter2>0) {	
	final_groups=(int **)malloc(counter2*sizeof(int *));
	assert((int) final_groups);
	for (i=0;i<counter2;i++) {
		final_groups[i]=(int *)malloc((ntax+1)*sizeof(int));
		assert((int) final_groups[i]);
		}
	}
/*start hennig output*/
printf("xread\n");
printf("'Tree generated by Jackbooting ala Farris with %d replicates\n",values->jackboot);
counter2=0;
for (i=0;i<counter;i++) if (num_of_each[i]>=((values->cutpoint*values->jackboot)/100)) counter2++;
printf("There were %d suported (>=%d%% replicates) groups of %d possible and non-trivial\n",counter2,values->cutpoint,ntax-2);
fprintf(stderr,"There were %d suported (>=%d%% replicates) groups of %d possible and non-trivial\n",counter2,values->cutpoint,ntax-2);

if (counter2>0) {
	counter2=0;
	for (i=0;i<counter;i++) {
		if (num_of_each[i]>=((values->cutpoint*values->jackboot)/100)) {
			printf("     Group %d %d%% (",counter2,(num_of_each[i]*100)/values->jackboot);
			fprintf(stderr,"     Group %d %d%% (",counter2,(num_of_each[i]*100)/values->jackboot);
			counter3=0;
			for (j=0;j<ntax;j++) {
				final_groups[counter2][j]=big_groups[i][j];
				if (big_groups[i][j]) {
					++counter3;
					fprintf(stderr,"%s",a[j]->name);
					if (counter3<big_groups[i][ntax]) fprintf(stderr," ");
					printf("%s",a[j]->name);
					if (counter3<big_groups[i][ntax]) printf(" ");
					}
				}
			final_groups[counter2][ntax]=big_groups[i][ntax];
			fprintf(stderr,")\n");
			printf(")\n");
			counter2++;
			}
		}
	}
/*get names of groups*/
group_names=(char *)malloc(FIFTEEN_BITS_MAX*sizeof(char));
assert((int)group_names);
temp1=(char *)malloc(FIFTEEN_BITS_MAX*sizeof(char));
assert((int)temp1);
counter3=1;
group_names[0]='(';
for (i=0;i<ntax;i++) {
	for (j=0;j<strlen(a[i]->name);j++) group_names[counter3++]=a[i]->name[j];
	group_names[counter3++]=' ';
	if (i==(ntax-1)) group_names[counter3++]=')';
	}
group_names[counter3++]='\0';
/*fprintf(stderr,"%s\n",group_names);*/
if (counter2>0) {
	for (i=ntax-1;i>1;i--) {
		for (j=0;j<counter2;j++) if (final_groups[j][ntax]==i) { /*add in and paren*/
			/*remove names in new group from group_name*/
			first=-1;
			for (k=0;k<ntax;k++) if (final_groups[j][k]) {
				/*remove name*/
				in=find_name(a[k]->name,group_names);
				/*fprintf(stderr,"In %d ",in);*/
				if (!in) {
					fprintf(stderr,"error in making name\n");
					exit(-1);
					}
				cur_len=strlen(group_names);
				for (l=in;l<cur_len-strlen(a[k]->name)-1;l++) group_names[l]=group_names[l+strlen(a[k]->name)+1];
				group_names[cur_len-strlen(a[k]->name)-1]='\0';
				/*if ((cur_len-(strlen(a[k]->name)-1)) != strlen(group_names)) fprintf(stderr,"Problems in termination\n");*/
				if (first<0) first = in;
				/*fprintf(stderr,"%d %s\n",k,group_names);*/
				}
			/*copy end to temp*/
			/*for (k=first;k<strlen(group_names);k++) temp1[k-first]=group_names[k];
			temp1[k]='\0';*/
			strcpy(temp1,group_names+first);
			/*fprintf(stderr,"gn %s\ntemp1 %s\n",group_names,temp1);*/
			/*add in parens and names*/
			/*fix for group stuff*/
			group_names[first++]='(';
			for (k=0;k<ntax;k++) if (final_groups[j][k]) {
				for (l=0;l<strlen(a[k]->name);l++) group_names[first++]=a[k]->name[l];
				group_names[first++]=' ';
				}
			group_names[first++]=')';
			group_names[first++]=' ';
			for (k=0;k<strlen(temp1);k++) group_names[first++]=temp1[k];
			group_names[first]='\0';
			/*fprintf(stderr,"%s\n",group_names);*/
			}
		}
	}
/*filter spaces*/
if (ba[0]) {if (ba[0]->name) free(ba[0]->name);}
else {
	ba[0]=make_align(a[0]);
	free(ba[0]->name);
	}
ba[0]->name=(char *)malloc((1+strlen(group_names))*sizeof(char));
assert((int) ba[0]->name);
counter3=0;
for (i=0;i<strlen(group_names);i++){
	if (group_names[i]!=' ') ba[0]->name[counter3++]=group_names[i];
	else if ((group_names[i]==' ') && (group_names[i+1]!=')')) ba[0]->name[counter3++]=group_names[i];
	}
ba[0]->name[counter3]='\0';
ba[0]->name=(char *)realloc(ba[0]->name,(1+strlen(ba[0]->name))*sizeof(char));
assert((int) ba[0]->name);
fprintf(stderr,"%s\n",ba[0]->name);
printf("%s\n",ba[0]->name);
if (counter2>0) counter3=print_jack(ba,1,values,a);
values->acgt=values->farris=values->inter_dig=values->dot=values->hen_gap=values->paup=values->paup_dot=0;
if (num_of_each) free(num_of_each);
if (group_names) free(group_names);
if (temp1) free(temp1);
if (big_groups) {
	for (i=0;i<counter;i++) if (big_groups[i]) free(big_groups[i]);
	free(big_groups);
	}
if (final_groups) {
	for (i=0;i<counter2;i++) if (final_groups[i]) free(final_groups[i]);
	free(final_groups);
	}
return num_there;
}

int **get_groups4(buffer,ntax,a,values,ngroups)
char *buffer;
int ntax;
alignment **a;
parameters *values;
int *ngroups;
{
int i,j,*buffer2,paren_check_left,paren_check_right;
int rep_length,groups_marker,groups_number;
int **groups;

i=((*ngroups))=0;

/*
fprintf(stderr,"%s\n",buffer);
*/
paren_check_left=paren_check_right=0;
for (i=0;i<strlen(buffer);i++) {
	if (buffer[i]=='(') ++paren_check_left;
	else if (buffer[i]==')') ++paren_check_right;
	}
if (paren_check_left!=paren_check_right) {
	fprintf(stderr,"Parentheses do not match in groups specification.\n");
	exit(-1);
	}

buffer2=(int *)malloc((strlen(buffer)+1)*sizeof(int));
assert((int)buffer2);
for (i=0;i<(strlen(buffer)+1);i++) buffer2[i]=-32000;
j=0;
for (i=0;i<strlen(buffer);i++)
	{
	if (buffer[i]=='(') buffer2[j++]=-1;
	else if (buffer[i]==')') buffer2[j++]=-2;
	else if (buffer[i]==',') buffer2[j++]=-3;
	else if (isspace(buffer[i])) buffer2[j++]=-3; /*can get rid of extra commas and terminal spaces laterif need to */
	else if (isalnum(buffer[i])) {
		get_taxon_number(&i,&j,buffer,buffer2,a,ntax);
		--i;
		}
	else {
		fprintf(stderr,"Bad character |%c| in position %d of groups description.\n",buffer[i],i);
		exit(-1);
		}
	}
buffer2[i]=-4;
i=0;
while (buffer2[i]>-4) {
	if (buffer2[i]==-2) rep_length=i+1;
	i++;
	}

/*
for (i=0;i<rep_length;i++) fprintf(stderr,"%d,",buffer2[i]);
fprintf(stderr,"\n");
*/

/*the real stuff*/
(*ngroups)=paren_check_left;
groups=(int **)malloc((*ngroups)*sizeof(int *));
assert((int)groups);
for (i=0;i<paren_check_left;i++) {
	groups[i]=(int *)malloc((ntax+1)*sizeof(int));
	assert((int)groups[i]);
	for (j=0;j<ntax+1;j++) groups[i][j]=0;
	}

groups_number=0;
for (i=0;i<rep_length;i++) {
	if (buffer2[i]==-1) {
		groups_marker=0;
		for (j=i;j<rep_length;j++) {
			if (buffer2[j]==-1) ++groups_marker;
			if ((buffer2[j]>=0)&&(buffer2[j]<ntax)) groups[groups_number][buffer2[j]]=1;
			if (buffer2[j]==-2) {
				--groups_marker;
				if (groups_marker==0) break;
				}
			}
		++groups_number;
		}
	}

for (i=0;i<(*ngroups);i++) {
	for (j=0;j<ntax;j++) groups[i][ntax]+=groups[i][j];
	if (groups[i][ntax]==0) {
		fprintf(stderr,"Error in groups specification -- parenthesis pair which contains no sequences.\n");
		exit(-1);
		}
	/*
	for (j=0;j<=ntax;j++) fprintf(stderr," %d ",groups[i][j]);
	fprintf(stderr,"\n");*/
	}
/*fprintf(stderr,"\n");*/

/*if (values->VERBOSE) fprintf(stderr,"%d groups required\n",(*ngroups));*/
free(buffer2);
return groups;
}

int find_name(s1,s2)
char *s1, *s2;
{
int i,j,found;

for (i=0;i< strlen(s2)-strlen(s1);i++) {
	found=1;
	for (j=0;j<strlen(s1);j++) {
		if (s2[i+j]!=s1[j]) {
			found=0;
			break;
			}
		}
	if (found) return(i);
	}
return 0;
}

int print_jack(best_aligns,count,values,a)
alignment **best_aligns, **a;
int count;
parameters *values;
{
int i,j,counter,pos;
char *buf;

if (values->farris || values->hen_gap) {

	if ((!values->delta)&&(!values->ttr)) printf(" %d gap cost %d change cost \n",values->gap_cost,values->change_cost);
	else if (values->delta) {
		printf(" %d gap cost \n",values->gap_cost);
		printf("cost matrix:\n	A	C	G	T \n");
		printf("A	%3d %3d %3d %3d\n",values->delta[0][0],values->delta[0][1],values->delta[0][2],values->delta[0][3]);
		printf("C	%3d %3d %3d %3d\n",values->delta[1][0],values->delta[1][1],values->delta[1][2],values->delta[1][3]);
		printf("G	%3d %3d %3d %3d\n",values->delta[2][0],values->delta[2][1],values->delta[2][2],values->delta[2][3]);
		printf("T	%3d %3d %3d %3d\n",values->delta[3][0],values->delta[3][1],values->delta[3][2],values->delta[3][3]);
		printf("\n");
		}
	else if (values->ttr) {
		printf(" %d gap cost \n",values->gap_cost);
		printf("	transition cost %d transversion cost %d\n",values->transition,values->transversion + values->transition);
		printf("\n");
		}
	if (values->dump_parameters) print_parameters(values);
	printf("'\n");

	printf("1 %d\n",values->all_done);
	if (!values->nona) {
		for (i = 0; i < 2; i++) printf("%s 0\n",a[i]->taxon_name[0]);
		for (i = 2; i < values->all_done; i++) printf("%s 1\n",a[i]->taxon_name[0]);
		}
	else {
		for (i = 0; i < 2; i++) {
			for (j=0;j<min(values->max_name_length,strlen(a[i]->taxon_name[0]));j++) printf("%c",a[i]->taxon_name[0][j]);
			printf(" 0\n");
			}
		for (i = 2; i < values->all_done; i++) {
			for (j=0;j<min(values->max_name_length,strlen(a[i]->taxon_name[0]));j++) printf("%c",a[i]->taxon_name[0][j]);
			printf(" 1\n");
			}
		}
	printf(";\n");
	printf("cc-.;\n");
	printf("tread\n");
	for (i=0;i<count;i++) {
		buf=(char *)malloc((1+strlen(best_aligns[i]->name))*sizeof(char));
		assert((int) buf);
		counter=0;
		pos=0;
		for (j=0;j<strlen(best_aligns[i]->name);j++) {
			if (best_aligns[i]->name[j]=='(') buf[pos++]=best_aligns[i]->name[j];
			else if ((best_aligns[i]->name[j]!=')') && (best_aligns[i]->name[j]!=' ')) {
				if (counter < values->max_name_length) buf[pos++]=best_aligns[i]->name[j];
				counter++;
				}
			else if ((best_aligns[i]->name[j]==')') || (best_aligns[i]->name[j]==' ')) {
				buf[pos++]=best_aligns[i]->name[j];
				counter=0;
				}
			}
		/*buf[pos]='\0';*/
		for (j=0;j<pos;j++) printf("%c",buf[j]);
		if (i<(count-1)) printf("*\n");
		else printf(";\n");
		free(buf);
		}
	printf("\nproc /	;\n");
	}
return 1;
}
	