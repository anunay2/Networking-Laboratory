#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


#define CR 0x0D
#define LF 0x0A
#define MAX_IN_PACK 200

///change this to user input later
unsigned char dest_mac[] = "\x08\x00\x27\xdc\x15\x89";
//mac of actual
int number;
unsigned char out_packet[64];
unsigned char c[2];
int handle,length,i;
unsigned char in_packet[300];
unsigned char my_mac[6];
unsigned char type[]="\xff\xff";
int rem,q;
int fac;
unsigned char broadcast_mac[] = "\xff\xff\xff\xff\xff\xff";
int count=0;
//int temp;
unsigned char temp;


void mymac();
void fill_packet_headers();
void fill_msg(unsigned char *msg , int length);
void send_packet();
int access_type();
int release_type();
void send_factorial();

void interrupt receiver(bp, di, si, ds, es, dx, cx, bx, ax, ip, cs, flags)
unsigned short bp, di, si, ds, es, dx, cx, bx, ax, ip, cs, flags;
{
        if(ax==0)
        {
              
                //storing the incoming packet or giving it the address
                es=FP_SEG(in_packet);
                di=FP_OFF(in_packet);
                length=cx;//length of the input packet will be stored in the cx register

        }
        else
        {
               //first filter out roadcasts
                if(memcmp(in_packet, broadcast_mac , 6)==0)
                {
           
                        return;
                }
                cprintf("Sender's MAC-ID:");
                for(i=6;i<12;i++)
                {
                        cprintf("%02x:", in_packet[i]);
                }
                 cprintf("--");
                 if(count==2){
                 cprintf("Caught the number ");
                 cprintf("%d\n",(int)in_packet[14]);
                 cprintf("Now I am sending factorial");
                 send_factorial((int)in_packet[14]);
                 count=0;
           
                }
                if(count==1){
                        cprintf("Factorial is:\n");
                        fac=(int)in_packet[15]*256+(int)in_packet[14];
                        cprintf("%d\n",fac);
                        count=0;
                }
                putch('\r');
                putch('\n');
                return;
        }
}
void send_factorial(int n){
        int i=1,p=1;
        for(i=1;i<=n;i++){
              p=p*i;
        }
        rem=p%256;
        q=p/256;
        out_packet[14]=(char)rem;
        out_packet[15]=(char)q;
        send_packet();
}

int main()
{
        mymac();
        fill_packet_headers();
        access_type();
        cprintf("F:Send Number and Receive Factorial");
        cprintf("R:Receive the number and Send Factorial");
        cprintf("X:Terminate");
        while(1)
        {             
                temp=getchar();
                if(temp=='X'){
                        break;
                }
                if(temp=='F'){
                     count=1;
                     cprintf("Please Enter the number:");
                     scanf("%d",&number);
                     out_packet[14]=(char)number;
                     send_packet();
                     cprintf("\nPacket Sent\n");
                                 
                }
                else if(temp=='R'){
                     count=2;
                }                   
        }
        release_type();
        cprintf("Had a Great Networking\n\r");
        return 0;
}

void mymac()
{
        union REGS inregs,outregs;
        struct SREGS segregs;
        char far *bufptr;
        segread(&segregs);
        bufptr = (char far *)my_mac;
        segregs.es = FP_SEG(bufptr);
        inregs.x.di = FP_OFF(my_mac);
        inregs.x.cx = 6;
        inregs.h.ah = 6;
        int86x(0x60, &inregs, &outregs, &segregs);
}

void fill_packet_headers()
{

        memcpy(out_packet,dest_mac, 6); //set the destination mac
        memcpy(out_packet+6, my_mac, 6); //set the source mac
        memcpy(out_packet+12, type, 2); //set the type
}

void send_packet()
{
        union REGS inregs,outregs;
        struct SREGS segregs;
        segread(&segregs);
        inregs.h.ah = 4;
        segregs.ds = FP_SEG(out_packet);
        inregs.x.si = FP_OFF(out_packet);
        inregs.x.cx = 64;
        int86x(0x60,&inregs,&outregs,&segregs);
}

int access_type()
{
        union REGS inregs,outregs;
        struct SREGS segregs;
        inregs.h.al=1; //if_class
        inregs.x.bx=-1;///try changing this
        inregs.h.dl=0; //if_number
        inregs.x.cx=0; //typelen =0
        inregs.h.ah=2; //interrupt number
        segregs.es=FP_SEG(receiver);
        inregs.x.di=FP_OFF(receiver);
        c[0]=0xff;
        c[1]=0xff;
        segregs.ds=FP_SEG(c);
        inregs.x.si=FP_OFF(c);
        int86x(0x60,&inregs,&outregs,&segregs);
       //cprintf("Access Type %d\n",outregs.x.cflag);
       //cprintf("Handle %d\n",outregs.x.ax);
        handle = outregs.x.ax;
        return outregs.x.cflag;
}

int release_type()
{
        union REGS inregs,outregs;
        struct SREGS segregs;
        inregs.h.ah=3;
        //passing the same handle returned by access_type function
        inregs.x.bx=handle;
        int86x(0x60,&inregs,&outregs,&segregs);
      //cprintf("Calling the release Type function to terminate the effect ofaccess type");
        return 0;
}
