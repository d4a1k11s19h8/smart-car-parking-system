#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

struct Parking_Lot {
    int P_id;
    int status;
    float parking_hours;
    float revenue;
};

struct Vehicle {
    char vehicle_num[11];
    char owner_name[50];
    char arrival_date[9];
    char arrival_time[5];
    char departure_date[9];
    char departure_time[5];
    int membership;
    float parking_hours;
    int P_id;
    int number_of_parks;
    float total_amount_paid;
};

struct NodeP {
    struct Parking_Lot P;
    struct NodeP *next;
} *parkfirst = NULL, *parklast = NULL;

struct NodeV {
    struct Vehicle V;
    struct NodeV *next;
} *vehiclefirst = NULL, *vehiclelast = NULL;

int str2int(char data[], int start, int space) {
    int retval = 0, i = start;
    while (i < space) {
        retval = retval * 10 + (data[i] - '0');
        i++;
    }
    return retval;
}

float str2float(char data[], int start, int space) {
    float retval = 0;
    int i = start, count = 0;
    int flag = 0;
    while (i < space) {
        if (data[i] == '.') {
            flag = 1;
            count = 1;
            i++;
        } else {
            if (!flag) {
                retval = retval * 10 + (data[i] - '0');
            } else {
                retval += (data[i] - '0') / pow(10, count);
                count++;
            }
            i++;
        }
    }
    return retval;
}

void Read_Parking_Lot() {
    char data[500];
    FILE *ptr = fopen("parking-lot-data.txt", "r");
    if (ptr == NULL) {
        printf("Error opening file.\n");
        return;
    }

    while (fgets(data, 500, ptr) != NULL) {
        struct NodeP *temp = (struct NodeP *)malloc(sizeof(struct NodeP));
        int i = 0, j = 0;
        while (data[j] != ' ') j++;
        temp->P.P_id = str2int(data, i, j);
        i = ++j;
        while (data[j] != ' ') j++;
        temp->P.status = str2int(data, i, j);
        i = ++j;
        while (data[j] != ' ') j++;
        temp->P.parking_hours = str2float(data, i, j);
        i = ++j;
        while (data[j] != '\n' && data[j] != '\0') j++;
        temp->P.revenue = str2float(data, i, j);
        temp->next = NULL;

        if (!parkfirst) {
            parkfirst = parklast = temp;
        } else {
            parklast->next = temp;
            parklast = temp;
        }
    }
    fclose(ptr);
}

void PrintP() {
    struct NodeP *p = parkfirst;
    while (p) {
        printf("%d %d %.2f %.2f\n", p->P.P_id, p->P.status, p->P.parking_hours, p->P.revenue);
        p = p->next;
    }
}

void Read_Vehicle_Data() {
    char data[200];
    FILE *ptr = fopen("complete-vehicle-database-100.txt", "r");
    if (!ptr) {
        printf("Error opening file.\n");
        return;
    }

    while (fgets(data, 200, ptr) != NULL) {
        struct NodeV *current = (struct NodeV *)malloc(sizeof(struct NodeV));
        int i = 0, j = 0;
        while (data[j] != ' ') j++;
        strncpy(current->V.vehicle_num, data + i, j - i);
        current->V.vehicle_num[j - i] = '\0';
        i = ++j;
        while (data[j] != ' ') j++;
        strncpy(current->V.owner_name, data + i, j - i);
        current->V.owner_name[j - i] = '\0';
        i = ++j;
        while (data[j] != ' ') j++;
        strncpy(current->V.arrival_date, data + i, j - i);
        current->V.arrival_date[j - i] = '\0';
        i = ++j;
        while (data[j] != ' ') j++;
        strncpy(current->V.arrival_time, data + i, j - i);
        current->V.arrival_time[j - i] = '\0';
        i = ++j;
        while (data[j] != ' ') j++;
        strncpy(current->V.departure_date, data + i, j - i);
        current->V.departure_date[j - i] = '\0';
        i = ++j;
        while (data[j] != ' ') j++;
        strncpy(current->V.departure_time, data + i, j - i);
        current->V.departure_time[j - i] = '\0';
        i = ++j;
        while (data[j] != ' ') j++;
        current->V.membership = str2int(data, i, j);
        i = ++j;
        while (data[j] != ' ') j++;
        current->V.parking_hours = str2float(data, i, j);
        i = ++j;
        while (data[j] != ' ') j++;
        current->V.P_id = str2int(data, i, j);
        i = ++j;
        while (data[j] != ' ') j++;
        current->V.number_of_parks = str2int(data, i, j);
        i = ++j;
        while (data[j] != '\n' && data[j] != '\0') j++;
        current->V.total_amount_paid = str2float(data, i, j);
        current->next = NULL;

        if (!vehiclefirst) {
            vehiclefirst = vehiclelast = current;
        } else {
            vehiclelast->next = current;
            vehiclelast = current;
        }
    }
    fclose(ptr);
}

void displayR() {
    struct NodeV *current = vehiclefirst;
    while (current) {
        printf("%s %s %s %s %s %s %d %.2f %d %d %.2f\n",
               current->V.vehicle_num, current->V.owner_name, current->V.arrival_date, 
               current->V.arrival_time, current->V.departure_date, current->V.departure_time,
               current->V.membership, current->V.parking_hours, current->V.P_id, 
               current->V.number_of_parks, current->V.total_amount_paid);
        current = current->next;
    }
}

int isAvailable(struct NodeP *current)
{
    int vacant=0;
    while(current && !vacant)
    {
        if(current->P.status==0)
        {
            vacant=1;
        }
        current=current->next;
    } 
    return vacant;
}

struct NodeV* isRegistered(struct NodeV *current,char vehicle_num[10])
{
   int registered=-1;
   struct NodeV* temp=NULL;
   while(current && registered==-1)
   {
      if(strcmp(current->V.vehicle_num,vehicle_num)==0)
      {
         registered=1;
         temp=current;
      }
      else
      current=current->next;     
   }
   return temp;
}

struct NodeP* vacantid(struct NodeP* current,int size,int membership)
{
    struct NodeP* temp=NULL;
    while(current)
    {
    if(current->P.status==0)
    {
    if(membership==0)
    {
        if(current->P.P_id>20)
        {
            if(!temp)
             temp=current;
            else
            {
                if(current->P.P_id<temp->P.P_id)
                 temp=current;
            }
        }
    }
    else if(membership==1)
    {
        if(current->P.P_id>10 && current->P.P_id<21)
        {
            if(!temp)
             temp=current;
            else
            {
                if(current->P.P_id<temp->P.P_id)
                 temp=current;
            }
        }
    }
    else
    {
       if(current->P.P_id>0 && current->P.P_id<11)
        {
            if(!temp)
             temp=current;
            else
            {
                if(current->P.P_id<temp->P.P_id)
                 temp=current;
            }
        }
    }
    current=current->next;
    }
    else
     current=current->next;
    }
   return temp;
}

float revenue(int hours,int membership){
    float revenue;
    if(hours>3)
    {
       revenue=100+50*ceil(hours-3);
        }
    else
      revenue=100;
    if(membership!=0)
    {
       revenue=0.9*revenue;
    }
    return revenue;
}

int sabs(int a ){
    int retval=a;
    if(a<0){
        retval=-retval;;
    }
    return retval;
}
int leap_year(int year) {
    int retval;
    if (year%4==0 && (year % 100 != 0 || year % 400 == 0))
        retval=1;
    else
        retval=0;
    return retval;
}

int no_of_days(char start_date[],char end_date[]) {
    int arr_date,dep_date,arr_month,dep_month,arr_year,dep_year,i;
    arr_date=0;
    dep_date=0;
    arr_month=0;
    dep_month=0;
    arr_year=0;
    dep_year=0;

    for(i=0; i<2; i++) {
        arr_date=arr_date*10+(int)(start_date[i]-'0');
        dep_date=dep_date*10+(int)(end_date[i]-'0');
    }
    for(i=2; i<4; i++) {
        arr_month=arr_month*10+(int)(start_date[i]-'0');
        dep_month=dep_month*10+(int)(end_date[i]-'0');
    }
    for(i=4; i<8; i++) {
        arr_year = arr_year * 10 + (int)(start_date[i]-'0');
        dep_year=dep_year*10+(int)(end_date[i]-'0');
    }
    //printf("%d %d %d %d %d %d \n",arr_date,arr_month,arr_year,dep_date,dep_month,dep_year);
    int month[]= {31,28,31,30,31,30,31,31,30,31,30,31};
    int diff_date=dep_date-arr_date;
    int diff_month=dep_month-arr_month;
    for(i=arr_year; i<dep_year; i++) {
        diff_date+=365+leap_year(arr_year+1) ;
    }//for leap year
    if (dep_month<3 && leap_year(dep_year)){
            diff_date+=-1;}
    if (arr_month<3 && leap_year(arr_year))
            diff_date+=1;
    if (diff_month<0) {
        int k=sabs(diff_month);
        for (i=0; i<k; i++)
            diff_date=diff_date-month[(12+arr_month-i-1)%12];}
    else {
        for (i=0; i<diff_month; i++)
            diff_date += month[arr_month + i];
            }

    int total_time=diff_date*24;
    // total_time=total_time+end_hrs-start_hrs
    //printf("%d\n",diff_date);
    return diff_date;
}

int numdigit(int a){
	int i=0;
	while(a!=0){
		i++;
		a=a/10;
	}
	return i;
}

void int2str(int a, char A[]) {
    int numdig=numdigit(a);
    int msb = 0;
    int i = 0;
    if(numdig==0){
        A[i++]='0';
    }
    while (numdig > 0) {
        msb = a / (int)pow(10, numdig - 1);
        A[i] = '0' + msb;
        a = (a - pow(10, numdig - 1) * msb);
        numdig--;
        i++;
    }
    
     A[i]='\0';
}

float diff_hrs(int diff_date,char start_time[],char end_time[]) {
    int start_hr,end_hr,start_min,end_min,i;
    start_hr=end_hr=start_min=end_min=0;
    for (i=0; i<2; i++) {
        start_hr=start_hr*10+(int)(start_time[i]-'0');
        end_hr=end_hr*10+(int)(end_time[i]-'0');
    }
    for(i=2; i<4; i++) {
        start_min=start_min*10+(int)(start_time[i]-'0');
        end_min=end_min*10+(int)(end_time[i]-'0');
    }

    int diff_hr=end_hr-start_hr+diff_date*24;
    int diff_min=end_min-start_min+diff_hr*60;
    float diff_total_hr=(float)diff_min/60;
    //printf("%d %d %d %d\n",start_hr,end_hr,start_min,end_min);
    //printf("%d %d %d\n",diff_hr, diff_min,diff_total_hr);
    return diff_total_hr;
}

void Vehicle_Entry(struct NodeP* vacant,struct NodeV* registered,char vehicle_num[10])
{
    vacant->P.status=1;
    if(!registered)
    {
        struct NodeV* latest;
        latest=(struct NodeV*)malloc(sizeof(struct NodeV));
        strcpy(latest->V.vehicle_num,vehicle_num);
        printf("Enter date of arrival :");
        scanf("%s",&latest->V.arrival_date);
        printf("Enter time of arrival :");
        scanf("%s",&latest->V.arrival_time);
        getchar();
        printf("Enter your Name :");
        scanf("%s",&latest->V.owner_name);
        latest->V.membership=0;
        latest->V.number_of_parks=0;
        latest->V.total_amount_paid=0;
        latest->V.parking_hours=0;
        //latest->V.P_id=park_index;
        strcpy(latest->V.departure_date,latest->V.arrival_date);
        strcpy(latest->V.departure_time,latest->V.arrival_time);
        printf("Your vehicle is parked at %d \n",vacant->P.P_id);
        printf("Congratulation Now you are a part of ParkingMaster! ");
        latest->V.P_id=vacant->P.P_id;
        vehiclelast->next=latest;
        latest->next=NULL;
        vehiclelast=latest;
    }
    else{
        printf("Enter date of arrival: ");
        scanf("%s",&registered->V.arrival_date);
        printf("Enter time of arrival: ");
        scanf("%s",&registered->V.arrival_time);
        registered->V.P_id=vacant->P.P_id;
        printf("Parking status updated! ");
        printf("Your vehicle is parked at %d",vacant->P.P_id);
    }
}

void Vehicle_Exit(int parking_id){
   //// temp_bubble(p);
    //displayP(p);
    struct NodeP* vacant=parkfirst;
    struct NodeV* registered=vehiclefirst;
    while(vacant && vacant->P.P_id!=parking_id)
     vacant=vacant->next;
    vacant->P.status=0;    //resets the status of parking lot
    while(registered && registered->V.P_id!=parking_id)
     registered=registered->next;
    printf("Enter departure date");
    scanf("%s",&registered->V.departure_date);
    printf("Enter departure time");
    scanf("%s",&registered->V.departure_time);
    float parkinghours=diff_hrs(no_of_days(registered->V.arrival_date,registered->V.departure_date),registered->V.arrival_time,registered->V.departure_time);
    //printf("%.2f ",parkinghours);
    registered->V.parking_hours+=parkinghours;
    //printf("%.2f ",registered->V.parking_hours);
    if(registered->V.parking_hours>=200)
    {
        registered->V.membership=2;
    }
    else if(registered->V.parking_hours>=100)
    {
        registered->V.membership=1;
    }
    registered->V.number_of_parks+=1;
    registered->V.P_id=0;
    registered->V.total_amount_paid+=revenue(ceil(parkinghours),registered->V.membership);
    vacant->P.parking_hours+=parkinghours;
    vacant->P.revenue+=revenue(ceil(parkinghours),registered->V.membership);
    printf("%.2f \n",revenue(ceil(parkinghours),registered->V.membership));
    //displayP(p);
  //  // QuickSortP(p,0,50);
    //displayP(p);
}

struct NodeV* partitionV(struct NodeV *list_ptr)
{
   struct NodeV*nptr,*fast,*slow;
   slow=list_ptr;
   fast=list_ptr->next->next;
   while(fast)
   {
    slow=slow->next;
    fast=fast->next;
    if(fast)
     fast=fast->next;
   }
   nptr=slow->next;
   slow->next=NULL;
   return nptr;
}

struct NodeV* MergeV(struct NodeV* list1,struct NodeV* list2)
{
    struct NodeV *result,*ptr1,*ptr2,*tail;
    ptr1=list1;
    ptr2=list2;
    if(list1->V.number_of_parks > list2->V.number_of_parks)
    {
       result=tail=list1;
       ptr1=ptr1->next;
    }
    else
    {
        result=tail=list2;
        ptr2=ptr2->next;
    }
    while(ptr1 && ptr2)
    {
        if(ptr1->V.number_of_parks > ptr2->V.number_of_parks)
        {
            tail->next=ptr1;
            tail=tail->next;
            ptr1=ptr1->next;
        }
        else
        {
            tail=tail->next=ptr2;
            ptr2=ptr2->next;
        }
    }
    if(ptr1)
     tail->next=ptr1;
    else
     tail->next=ptr2;
    return result;
}

struct NodeV* MergeSortV(struct NodeV *lptr)
{
    struct NodeV *nptr,*list_ptr;
    list_ptr=lptr;
    if(lptr!=NULL && lptr->next!=NULL)
    {
        nptr=partitionV(list_ptr);
        list_ptr=MergeSortV(list_ptr);
        nptr=MergeSortV(nptr);
        lptr=MergeV(list_ptr,nptr);
    }
    return lptr;
}

struct NodeP* partitionP(struct NodeP *list_ptr)
{
   struct NodeP*nptr,*fast,*slow;
   slow=list_ptr;
   fast=list_ptr->next->next;
   while(fast)
   {
    slow=slow->next;
    fast=fast->next;
    if(fast)
     fast=fast->next;
   }
   nptr=slow->next;
   slow->next=NULL;
   return nptr;
}

struct NodeP* MergeP(struct NodeP* list1,struct NodeP* list2)
{
    struct NodeP *result,*ptr1,*ptr2,*tail;
    ptr1=list1;
    ptr2=list2;
    if(list1->P.parking_hours > list2->P.parking_hours)
    {
       result=tail=list1;
       ptr1=ptr1->next;
    }
    else
    {
        result=tail=list2;
        ptr2=ptr2->next;
    }
    while(ptr1 && ptr2)
    {
        if(ptr1->P.parking_hours > ptr2->P.parking_hours)
        {
            tail->next=ptr1;
            tail=tail->next;
            ptr1=ptr1->next;
        }
        else
        {
            tail=tail->next=ptr2;
            ptr2=ptr2->next;
        }
    }
    if(ptr1)
     tail->next=ptr1;
    else
     tail->next=ptr2;
    return result;
}

struct NodeP* MergeSortP(struct NodeP *lptr)
{
    struct NodeP *nptr,*list_ptr;
    list_ptr=lptr;
    if(lptr!=NULL && lptr->next!=NULL)
    {
        nptr=partitionP(list_ptr);
        list_ptr=MergeSortP(list_ptr);
        nptr=MergeSortP(nptr);
        lptr=MergeP(list_ptr,nptr);
    }
    return lptr;
}

void WriteP(struct NodeP* current)
{
   FILE *ptr = fopen("parking-lot-data.txt", "w");
   if (ptr == NULL) {
       printf("Error opening file!\n");
       return;
   }

   struct NodeP* temp;

   while (current)
   {   
        char str[100];
        int2str(current->P.P_id, str);
        fputs(str, ptr);
        fputs(" ", ptr);
        int2str(current->P.status, str);
        fputs(str, ptr);
        fputs(" ", ptr);
        gcvt(current->P.parking_hours, 5, str);
        fputs(str, ptr);
        fputs(" ", ptr);
        gcvt(current->P.revenue, 5, str);
        fputs(str, ptr);
        fputs("\n", ptr);

        // Freeing memory
        temp = current;
        current = current->next;
        free(temp);
   }

   fclose(ptr);
}

void WriteV(struct NodeV* current)
{
    FILE *ptr = fopen("complete-vehicle-database-100.txt", "w");
    if (ptr == NULL) {
        printf("Error opening file!\n");
        return;
    }

    struct NodeV* temp;

    while (current)
    {
        char str[20];

        fputs(current->V.vehicle_num, ptr);
        fputs(" ", ptr);
        fputs(current->V.owner_name, ptr);
        fputs(" ", ptr);
        fputs(current->V.arrival_date, ptr);
        fputs(" ", ptr);
        fputs(current->V.arrival_time, ptr);
        fputs(" ", ptr);
        fputs(current->V.departure_date, ptr);
        fputs(" ", ptr);
        fputs(current->V.departure_time, ptr);
        fputs(" ", ptr);
        int2str(current->V.membership, str);
        fputs(str, ptr);
        fputs(" ", ptr);
        gcvt(current->V.parking_hours, 10, str);
        fputs(str, ptr);
        fputs(" ", ptr);
        int2str(current->V.P_id, str);
        fputs(str, ptr);
        fputs(" ", ptr);
        int2str(current->V.number_of_parks, str);
        fputs(str, ptr);
        fputs(" ", ptr);
        gcvt(current->V.total_amount_paid, 10, str);
        fputs(str, ptr);
        fputs("\n", ptr);

        // Free memory
        temp = current;
        current = current->next;
        free(temp);
    }

    fclose(ptr);
}


struct NodeV* swapV(struct NodeV* ptr1, struct NodeV* ptr2) {
    struct NodeV* tmp = ptr2->next;
    ptr2->next = ptr1;
    ptr1->next = tmp;
    return ptr2;
}

void bubbleSortV(struct NodeV** head) {
    if (*head == NULL || (*head)->next == NULL)
        return;

    struct NodeV** h;
    int swapped=1;
    
    for (int i = 0;swapped ; i++) {
        h = head;
        swapped = 0;
        
        for (int j = 0; j < i; j++) {
            if (*h == NULL || (*h)->next == NULL)
                return;
            h = &(*h)->next;
        }
        
        while (*h && (*h)->next) {
            if(((*h)->V.number_of_parks==(*h)->next->V.number_of_parks && (*h)->V.total_amount_paid<(*h)->next->V.total_amount_paid)){
                *h = swapV(*h, (*h)->next);
                swapped = 1;
            }
            h = &(*h)->next;
        }
    }
}

struct NodeP* swapP(struct NodeP* ptr1, struct NodeP* ptr2) {
    struct NodeP* tmp = ptr2->next;
    ptr2->next = ptr1;
    ptr1->next = tmp;
    return ptr2;
}

void bubbleSortP(struct NodeP** head) {
    if (*head == NULL || (*head)->next == NULL)
        return;

    struct NodeP** h;
    int swapped=1;
    
    for (int i = 0;swapped ; i++) {
        h = head;
        swapped = 0;
        
        for (int j = 0; j < i; j++) {
            if (*h == NULL || (*h)->next == NULL)
                return;
            h = &(*h)->next;
        }
        
        while (*h && (*h)->next) {
            if(((*h)->P.parking_hours==(*h)->next->P.parking_hours && (*h)->P.revenue>(*h)->next->P.parking_hours)){
                *h = swapP(*h, (*h)->next);
                swapped = 1;
            }
            h = &(*h)->next;
        }
    }
}



int main() {
    Read_Parking_Lot();
    Read_Vehicle_Data();
    parkfirst=MergeSortP(parkfirst);
    vehiclefirst=MergeSortV(vehiclefirst);
    int flag;
    printf("Do u want to use our Services(1 for yes or 0 for no)? ");
    scanf("%d",&flag);
    getchar();
    while(flag==1)
    {
    printf("Hola!Are u Entering(e) or leaving(l) or display(d) the ParkMaster! ");
    // getchar();
    char choice; 
    scanf(" %c",&choice);
    if(choice=='e')
    {
    printf("Entry Section \n");
    if(isAvailable(parkfirst)==1){
        char vehicle_num[10];
        printf("Welcome to the ParkMaster,One place Parking Solution for all your vehicles! \n");
        printf("Enter your Vehicle Number: ");
        scanf("%s",&vehicle_num);
        struct NodeV* register_index=isRegistered(vehiclefirst,vehicle_num);//Checks if vehicle is already registered or not
        int membership;
         if(register_index==NULL){      //assigns membership for new user
            membership=0;
        }
        else{                        //checks membership of existing user
            membership=register_index->V.membership;
        }
        struct NodeP* vacant_id=vacantid(parkfirst,50,membership); 
        //it checks vacant space in parking lot according to membership //VACANT_ID=21
        if(vacant_id==NULL)                      //parking is not available for the membership 
        {
            printf("Sorry No space available for you! \n");
        }
        else{                  //updates the parkmaster 
             Vehicle_Entry(vacant_id,register_index,vehicle_num);
           }
        printf("\n Do u want to use our Services(1 for yes or 0 for no)? ");
        scanf("%d",&flag);
        }
        else
        {
            printf("Sorry no space Available");
            flag=0;
            
        }
    }
        else if(choice=='l'){
        printf("Exit Section \n");
        int P_id;
        printf("Enter your parking Id: ");
        scanf("%d",&P_id);
        Vehicle_Exit(P_id);
        bubbleSortV(&vehiclefirst);
        bubbleSortP(&parkfirst);
        printf("\n Do u want to use our Services(1 for yes or 0 for no)? ");
        scanf("%d",&flag);
    }
else if(choice=='d')
    {
       PrintP();
       displayR();
       printf("\n Do u want to use our Services(1 for yes or 0 for no)? ");
       scanf("%d",&flag);
    } 
else{
        flag=0;
    }
    
    }
    
    WriteP(parkfirst);
    WriteV(vehiclefirst);
    return 0;
    }
    

