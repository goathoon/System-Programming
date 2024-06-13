#include "sfp.h"
#include <stdlib.h>
sfp int2sfp(int input){
	sfp c = 0;
    //input = 0 일때 , sfp는 0이어야함 (비트가 0000...000)
    if( input == 0)
        return 0;
    
    //int가 양수이면서 unsigned short 범위에 들어갈 수 있을 때 (뒤에서 16번째 비트부터 맨 MSB 비트까지 모두다 0이어야함 short로 바뀌면서 앞의 비트가 다 버려지므로)
    else if (input >0 && input >>15 == 0){

        int power = 0; //2의 지수부를 나타내기 위한 변수
        int exp = 0; //sfp 의 exp부분을 나타내기위한 int형 변수

        int frac_temp[10] = {0,}; //0으로 초기화해야함.
        int frac[10] = {0,};
        int cnt = 0; //계산 횟수
        while(1)
        {
            if( input / 2 !=0)
            {
                power ++;
                cnt++;
                if(cnt > 10)
                {
                	for(int i = 9; i>0; i--)
                	{
                		frac_temp[i] = frac_temp[i-1];
                	}
                	frac_temp[0] = input % 2; //frac 임시배열 (실제 frac bit과 순서는같음 위치만 다를 뿐) input을 2로나눈 나머지를 끝에서 부터 차곡차곡 저장한다.
                	input /=2;
            	}
            	else
            	{
            		frac_temp[10-cnt] = input%2;
            		input/=2;
            	}
            }
            else
            {
                break;
            }  
        }

        //이러면 frac_temp[10-cnt] 부터 유효한 frac이 된다. 따라서 다음과 같은 코드로 frac의 정렬
        int idx = 0;
        if(cnt <= 10)
        {
        
		for(int k = cnt; k >=1 ; k--)
		{
		    frac[idx] =frac_temp[10-k];
		    idx++;
		}
	}
	else
	{
		for(int k = 0; k<10; k++)
			frac[k] = frac_temp[k];
	}
        exp = power + 15; //exp표현
        c += exp;
        c <<= 10; // 0으로 표현된 sfp에 exp를 더하고나서 이 exp에 속하는 비트들을 10번 왼쪽으로 shift하면 exp부분에 비트들이 알맞게 들어가게 된다.

        //sfp의 frac부분에 알맞은 수를 입력하기 위한 for문이다.
        for(int i = 0; i<10 ; i++){
            c += (frac[i] << 9-i); 
        }
    }
    
    //int가 음수이면서 unsigned short 범위에 들어갈 수 있을 때 (~input을 하면 input값의 비트들이 모두 반대비트로 변환. short로 표현될수있는 값의 범위는 input의 앞 32비트가 모두 1이어야하는 조건을 포함
    else if( input < 0 && ~input >> 15 == 0)
    {
        int power = 0; //2의 지수부를 나타내기 위한 변수
        int exp = 0; //sfp 의 exp부분을 나타내기위한 int형 변수

        int frac_temp[10] = {0,}; //0으로 초기화해야함.
        int frac[10] = {0,};
        int cnt = 0; //계산 횟수
        int plus_input = -input;
        while(1)
        {
            if( plus_input / 2 !=0)
            {
                power ++;
                cnt++;
                if(cnt > 10)
                {
                	for(int i = 9; i>0; i--)
                	{
                		frac_temp[i] = frac_temp[i-1];
                	}
                	frac_temp[0] = plus_input % 2; //frac 임시배열 (실제 frac bit과 순서는같음 위치만 다를 뿐) input을 2로나눈 나머지를 끝에서 부터 차곡차곡 저장한다.
                	plus_input /=2;
            	}
            	else
            	{
            		frac_temp[10-cnt] = plus_input%2;
            		plus_input/=2;
            	}
            }
            else
            {
                break;
            }  
        }
        //이러면 frac_temp[10-cnt] 부터 유효한 frac이 된다. 따라서 다음과 같은 코드로 frac의 정렬
        int idx = 0;
        if(cnt <= 10)
        {
        
		for(int k = cnt; k >=1 ; k--)
		{
		    frac[idx] =frac_temp[10-k];
		    idx++;
		}
	}
	else
	{
		for(int k = 0; k<10; k++)
			frac[k] = frac_temp[k];
	}
        exp = power + 15; //exp표현
        c += exp;
        c <<= 10; // 0으로 표현된 sfp에 exp를 더하고나서 이 exp에 속하는 비트들을 10번 왼쪽으로 shift하면 exp부분에 비트들이 알맞게 들어가게 된다.

        //sfp의 frac부분에 알맞은 수를 입력하기 위한 for문이다.
        for(int i = 0; i<10 ; i++){
            c += (frac[i] << 9-i); 
        }

        c+= (1 << 15); //input이 음수이므로 반드시 MSB를 -1로 설정해야함.
    }

    //양수이면서 sfp범위에 벗어날 때,(1.1111111111 * 2^15 보다 클 때)
    else if( input >0 && input > (1<<16) -32 )
        c += (31<<10); //exp부분만 11111로 바꿈
    //음수이면서 sfp범위에 벗어날 때
    else if( input <0 && (input*-1) >(1>>16)-32)
        c += (63<<10); //exp부분 포함 signbit또한 1로 바뀌어야하므로 63 * 2^10적용
    
    return c;
}

int sfp2int(sfp input){
    int ans = 0;
    int sign;
    int add_sign = 1;
    int exp;
    int frac;
    int fracBit[10] = {0,};

    sign = input>>15;

    if(sign == 1)//음수비트가 있는경우, 즉 input의 맨 앞의 비트가 1인경우
    {
        exp = (input - (1<<15)) >>10; 
        add_sign = -1;
    }
    else
        exp = input >> 10;
    
    frac = input%(1<<10); //input에서 2^10 을 나누면, frac에 해당하는 sfp의 값이 나온다.

    //denormalized
    if(exp==31)
    {
        //NaN
        if(frac != 0)
        {
            return -2147483647 -1;
        }
        if(add_sign == 1)
        {
            return 2147483647;
        }
        else
            return -2147483647 -1;
    }
    
    int idx = 0;
    for(int i = 9; i>0; i--)
    {
        fracBit[idx] = frac>>i;
        idx++;
        if(frac>>i == 1)
            frac -=(1<<i);
    }

    for(int i = 1; i<=10; i++)
    {
    	if(exp-15-i >= 0)
        	ans += (fracBit[i-1] << (exp-15-i));
        else
        	ans += (fracBit[i-1] >> (15+i-exp));
    }
    ans += 1<<(exp-15); //leading 1 포함
    ans *= add_sign;
    return ans; //알아서 toward zero
}

sfp float2sfp(float input){
    sfp c = 0;
    float dec_below = input - (int)input; //소수점 아래만 따로 분리
    int integer = (int) input; //정수부분만 따로 분리
    //input = 0 일때 , sfp는 0이어야함 (비트가 0000...000)
    if( input == 0)
        return 0.0;
    
    //양수
    else if (input > 0 && integer >> 15 == 0)
    {
        int power = 0;
        int exp = 0;

        int frac_tempint[10] = {0,}; //소수부 frac 일시저장
        int frac_tempdec[10] = {0,}; //정수부 frac 일시저장
        int frac[10] = {0,};
        int cnt_int = 0;
        int cnt_dec = 0;
        float dec_div = 0.5; //소수점아래를 sfp로 나타내기 위한 나누는 수 정의
        //정수부분이 2보다 큰경우 power(2의 지수부를 증가시켜야함)증가
        while(1)
        {
            if (integer / 2 != 0)
            {
                power++;
                cnt_int++;
                if(cnt_int >10)
                {
                	for(int i = 9; i>0; i--)
                	{
                		frac_tempint[i] = frac_tempint[i-1];
                	}
                	
                	frac_tempint[0] = integer % 2;
                	integer /= 2;
            	}
            	else
            	{
            		frac_tempint[10-cnt_int] = integer%2;
            		integer /=2;
            	}
            }
            //정수부분이 1인경우 power는 변화 없음.
            else if (integer == 1)
            {
                if(dec_below ==0 ||(cnt_int + cnt_dec) >=10)
                    break;
                while(1)
                {
                    if ((cnt_int+cnt_dec) == 10 || dec_below == 0) // 이미 소수점아래가 다 표현이 되었거나, cnt=10즉, sfp의 frac범위를 초과한 경우 추가되는 수들을 버림 =round to zero
                        break;
                    else if (dec_below / dec_div >= 1)
                    {
                        frac_tempdec[10 - (++cnt_dec)] = 1;
                        dec_below -= dec_div;
                        dec_div /= 2;
                    }
                    else
                    {
                        dec_div /= 2;
                        frac_tempdec[10 - (++cnt_dec)] = 0;
                    }
                }
                
            }
            else
                break;
        }
        //정수부분이 0인경우 이경우는 처음부터 정수부분이 존재하지 않았던 경우임. 따라서 exp재정의 필요
        if(integer == 0)
        {
            power--;
            // exp 즉 2의 지수부를 찾을 때 까지 반복
            while (1)
            {
                if (power <= -15) //-->denormalized 로
                {
                    if (input < (1 >> 24)) // denormalized로도 표현할수 x 따라서 0으로 출력
                    {
                        return 0;
                    }
                    int cnt_dnorm = 9;
                    while (cnt_dnorm >= 0)
                    {
                        if (dec_below / dec_div >= 1)
                        {
                            c += 1 << cnt_dnorm;
                            dec_div /= 2;
                            cnt_dnorm--;
                        }
                        else
                        {
                            dec_div /= 2;
                            cnt_dnorm--;
                        }
                    }
                    return c;
                }
                else if (dec_below / dec_div >= 1) //처음으로 지수부를 찾음.
                {
                    dec_below -= dec_div;
                    break;
                }
                else
                {
                    power--;
                    dec_div /= 2;
                }
            }

            //-------여기서부터 integer =0 일 때, 소수부분 frac저장 코드
            cnt_dec++;
            dec_div /= 2;
            //소수부분도 frac_temp에저장 마찬가지로 역순
            while (cnt_dec <= 10 && dec_below != 0)
            {
                if (dec_below / dec_div >= 1)
                {
                    if(cnt_dec>10)
                    {
                    	for(int i = 9; i>0; i--)
                    	{
                    		frac_tempdec[i] = frac_tempdec[i-1];
                    	}
                    	frac_tempdec[0] = 1;
                    	dec_below -=dec_div;	 
                    }
                    else
                    {
                    	frac_tempdec[10 - cnt_dec] = 1;
                    	dec_below -= dec_div;
                    }
                }
                else
                {
                    if(cnt_dec>10)
                    {
                    	for(int i = 9; i>0; i--)
                    	{
                    		frac_tempdec[i] = frac_tempdec[i-1];
                    	}
                    	frac_tempdec[0] = 0;
                    	dec_below -=dec_div;	
                    }
                    else
                    {	
                    	frac_tempdec[10 - cnt_dec] = 0;
                    }	
                }	
                cnt_dec++;
                dec_div /= 2;

            } //이렇게하면 자동으로 toward-zero rounding 이 됨. sfp범위에 벗어나는 자리는 없애므로.
        }

        //------여기서부터 sfp값 'c'계산하는 과정-----------
        int idx = 0;
        if(cnt_int + cnt_dec <= 10)
        {
        	
		for(int k = cnt_int; k >=1 ; k--) //정수부분 frac_temp는 그 순서 그대로 frac에 저장
		{
		    frac[idx] =frac_tempint[10-k];
		    idx++;
		}
		for(int j = 1; j<=cnt_dec; j++)
		{
			frac[idx] = frac_tempdec[10-j];
			idx++;
		}
	}
	else if(cnt_int > 10)
	{
		for(int i =0; i<10; i++)
		{
			frac[idx] = frac_tempint[i];
			idx++;
		}	
	}
	
	else if(cnt_dec > 10)
	{
		for(int i = 0; i<10; i++)
		{
			frac[idx] = frac_tempdec[i];
			idx++;
		}
	}	
        exp = power + 15; //exp표현
        c += exp;
        c <<= 10; // 0으로 표현된 sfp에 exp를 더하고나서 이 exp에 속하는 비트들을 10번 왼쪽으로 shift하면 exp부분에 비트들이 알맞게 들어가게 된다.

        //sfp의 frac부분에 알맞은 수를 입력하기 위한 for문이다.
        for(int i = 0; i<10 ; i++){
            c += (frac[i] << 9-i); 
        }
    }
    else if( input <0 && ~integer >>15 ==0)
    {
        int power = 0;
        int exp = 0;

        int frac_tempint[10] = {0,}; //소수부 frac 일시저장
        int frac_tempdec[10] = {0,}; //정수부 frac 일시저장
        int frac[10] = {0,};
        int cnt_int = 0;
        int cnt_dec = 0;
        float dec_div = 0.5; //소수점아래를 sfp로 나타내기 위한 나누는 수 정의
        int plus_integer = -integer;
        //여전히 소수부분이 음수이므로 바꿔줘야함
        dec_below *= -1;
        while(1)
        {
            if (plus_integer / 2 != 0)
            {
                power++;
                cnt_int++;
                if(cnt_int >10)
                {
                	for(int i = 9; i>0; i--)
                	{
                		frac_tempint[i] = frac_tempint[i-1];
                	}
                	
                	frac_tempint[0] = plus_integer % 2;
                	plus_integer /= 2;
            	}
            	else
            	{
            		frac_tempint[10-cnt_int] = plus_integer%2;
            		plus_integer /=2;
            	}
            }	
            //정수부분이 1인경우 power는 변화 없음.
            else if (plus_integer == 1)
            {
                if(dec_below ==0 ||(cnt_int + cnt_dec) >=10)
                    break;
                while(1)
                {
                    if ((cnt_int+cnt_dec) == 10 || dec_below == 0) // 이미 소수점아래가 다 표현이 되었거나, cnt=10즉, sfp의 frac범위를 초과한 경우 추가되는 수들을 버림 =round to zero
                        break;
                    else if (dec_below / dec_div >= 1)
                    {
                        frac_tempdec[10 - (++cnt_dec)] = 1;
                        dec_below -= dec_div;
                        dec_div /= 2;
                    }
                    else
                    {
                        dec_div /= 2;
                        frac_tempdec[10 - (++cnt_dec)] = 0;
                    }
                }
                
            }
            else
                break;
        }
        //정수부분이 0인경우 이경우는 처음부터 정수부분이 존재하지 않았던 경우임. 따라서 exp재정의 필요
        if(plus_integer == 0)
        {
            power--;
            // exp 즉 2의 지수부를 찾을 때 까지 반복
            while (1)
            {
                if (power <= -15) //-->denormalized 로
                {
                    if (input < (1 >> 24)) // denormalized로도 표현할수 x 따라서 0으로 출력
                    {
                        return 0;
                    }
                    int cnt_dnorm = 9;
                    while (cnt_dnorm >= 0)
                    {
                        if (dec_below / dec_div >= 1)
                        {
                            c += 1 << cnt_dnorm;
                            dec_div /= 2;
                            cnt_dnorm--;
                        }
                        else
                        {
                            dec_div /= 2;
                            cnt_dnorm--;
                        }
                    }
                    return c;
                }
                else if (dec_below / dec_div >= 1) //처음으로 지수부를 찾음.
                {
                    dec_below -= dec_div;
                    break;
                }
                else
                {
                    power--;
                    dec_div /= 2;
                }
            }

            //-------여기서부터 plus_integer =0 일 때, 소수부분 frac저장 코드
            cnt_dec++;
            dec_div /= 2;
            //소수부분도 frac_temp에저장 마찬가지로 역순
            while (cnt_dec <= 10 && dec_below != 0)
            {
                if (dec_below / dec_div >= 1)
                {
                    if(cnt_dec>10)
                    {
                    	for(int i = 9; i>0; i--)
                    	{
                    		frac_tempdec[i] = frac_tempdec[i-1];
                    	}
                    	frac_tempdec[0] = 1;
                    	dec_below -=dec_div;	 
                    }
                    else
                    {
                    	frac_tempdec[10 - cnt_dec] = 1;
                    	dec_below -= dec_div;
                    }
                }
                else
                {
                    if(cnt_dec>10)
                    {
                    	for(int i = 9; i>0; i--)
                    	{
                    		frac_tempdec[i] = frac_tempdec[i-1];
                    	}
                    	frac_tempdec[0] = 0;
                    	dec_below -=dec_div;	
                    }
                    else
                    {	
                    	frac_tempdec[10 - cnt_dec] = 0;
                    }	
                }	
                cnt_dec++;
                dec_div /= 2;

            } //이렇게하면 자동으로 toward-zero rounding 이 됨. sfp범위에 벗어나는 자리는 없애므로.
        }

        //------여기서부터 sfp값 'c'계산하는 과정-----------
        int idx = 0;
        if(cnt_int + cnt_dec <= 10)
        {
        	
		for(int k = cnt_int; k >=1 ; k--) //정수부분 frac_temp는 그 순서 그대로 frac에 저장
		{
		    frac[idx] =frac_tempint[10-k];
		    idx++;
		}
		for(int j = 1; j<=cnt_dec; j++)
		{
			frac[idx] = frac_tempdec[10-j];
			idx++;
		}
	}
	else if(cnt_int > 10)
	{
		for(int i =0; i<10; i++)
		{
			frac[idx] = frac_tempint[i];
			idx++;
		}	
	}
	
	else if(cnt_dec > 10)
	{
		for(int i = 0; i<10; i++)
		{
			frac[idx] = frac_tempdec[i];
			idx++;
		}
	}	
        exp = power + 15; //exp표현
        c += exp;
        c <<= 10; // 0으로 표현된 sfp에 exp를 더하고나서 이 exp에 속하는 비트들을 10번 왼쪽으로 shift하면 exp부분에 비트들이 알맞게 들어가게 된다.

        //sfp의 frac부분에 알맞은 수를 입력하기 위한 for문이다.
        for(int i = 0; i<10 ; i++){
            c += (frac[i] << 9-i); 
        }

        c+= (1 << 15); //input이 음수이므로 반드시 MSB를 -1로 설정해야함.    
    }

    else if( input >0 && input > (1<<16) -32 )
        c += (31<<10); //exp부분만 11111로 바꿈
    //음수이면서 sfp범위에 벗어날 때
    else if( input <0 && (input*-1) >(1>>16)-32)
        c += (63<<10); //exp부분 포함 signbit또한 1로 바뀌어야하므로 63 * 2^10적용
    
    return c;
}

float sfp2float(sfp input){
    float ans = 0.0;
    int sign;
    int add_sign = 1;
    int exp;
    int frac;
    int fracBit[10] = {0,};

    sign = input>>15;

    if (sign ==1)
    {
        exp = (input - (1<<15))>>10;
        add_sign = -1;
    }
    else
        exp = input >> 10;

    frac= input%(1<<10);

    //denormalized
    if(exp==31)
    {
        if(frac !=0)
            return 0.0/0.0;
        else
            return 1.0/0.0;
    }

    int idx = 0;
    for(int i = 9; i>=0; i--)
    {
        fracBit[idx] = frac>>i;
        idx++;
        if(frac>>i ==1)
            frac -=(1<<i);
    }
    
    if(exp-15>=0)
    {
        for(int i = 1; i<= 10; i++)
        {
            if(exp-15-i>=0)
                ans += (float)fracBit[i-1] * (1<<(exp-15-i));
            else
                ans += (float)fracBit[i-1] / (1<<(i+15-exp));
        }
        ans += 1<<(exp-15);
        ans*=add_sign;
    }
    else
    {
        for(int i = 1; i<=10; i++)
        {
            ans+= (float)fracBit[i-1] / (1<<(i+15-exp));
        }
        ans+= (float)1/(1<<(15-exp));
        ans*= add_sign;
    }
    return ans;
}

sfp sfp_add(sfp a, sfp b){
    
    sfp c = 0;
	int signbit_a = 0; //a의 부호를 양으로 초기화
    int signbit_b = 0; //b의 부호를 양으로 초기화
    int signbit_result = 0;
    int exp_a;
    int exp_b;
    int exp_result = 0;
    if (a>>15 ==1)
    {   
        signbit_a = 1;
        exp_a = (a-(signbit_a * (1<<15))>>10);
    }
    else
        exp_a = (a>>10);
    if (b>>15 ==1)
    {   
        signbit_b = 1;
        exp_b = (b-(signbit_b * (1<<15))>>10);
    }
    else
        exp_b = (b>>10);
    

    int frac_a;
    int frac_b;
    int frac_result = 0;
    frac_a = a % (1<<10);
    frac_b = b % (1<<10);

    //---------------예외 사항들------------------//
    if(exp_a ==31 && frac_a ==0 && signbit_a ==0)
    {
        if(exp_b ==31 && frac_b ==0
         && signbit_b ==0)
            return 31<<10; //무한대 (exp부분만 11111인경우)
        else if(exp_b==31 && frac_b ==0 && signbit_b ==1)
            return (31<<10) + 1; //1은 NaN임을 알리기 위해 그냥 넣은것임
        else if(exp_b <31)
            return (31<<10);  
    }

    else if(exp_a ==31 && frac_a ==0 && signbit_a ==1)
    {
        if(exp_b ==31 && frac_b ==0 && signbit_b ==1)
            return 63<<10; //-무한대
        else if(exp_b<31)
            return 63<<10;
    }

    else if(exp_a ==31 && frac_a!=0)
        return (31<<10) + 1;
    //----------------예외 사항 끝------------------//

    //a의 지수부가 b의 지수부보다 클때  
    if(exp_a >exp_b)
    {
        int cnt = 0;
        float rounding = 0; //rounding 위한 저장 변수
        int rnd = 0;//rounding 위하여 rounding의 값의 정확한값을 알려주는 변수( 값이 버려져서 밀리므로)
        exp_result = exp_a;
        while(exp_a != exp_b)
        {
            exp_b++;

            //sfp 범위를 벗어나는 경우 round to even 실행
            if(frac_b%2 ==1) 
            {
                rounding += 1<<rnd; 
            }

            frac_b>>=1;
            if(cnt==0)
                frac_b += 1<<9; //leading '1'이 포함되어야함 처음한번만 포함해주면됨.
            cnt++;
            if(rounding >=1)
                rnd++; //rounding 위하면 반드시 필요 (버려지는 값을 저장해야 하므로)
        }

        //-------------------------------b 의 round to even -----------------------//
        if(rounding>=1)
        {
            if (rounding / (1 << rnd) > 0.5) // rounding이 1/2 초과인지 미만인지 같은지 판단해야 to even할 수 있음
                frac_b += 1;
            else if (rounding / (1 << rnd) == 0.5)
            {
                if (frac_b % 2 == 1) //마지막비트가 홀수 (짝수일때는 그냥 버리면 됨.)
                    frac_b += 1;
            }
        }

        //-------------------------------b 의 round to even -----------------------//
        if(signbit_a ==1)//a가 음수인 경우
        {
            if(signbit_b ==1)
            {
                frac_result = frac_a + frac_b;
                if(frac_result /(1<<10) >=2)//exp의 조정이 필요 a와 b의 합이 이진법의 유효 10자리 보다 큰경우를 조건문으로 제시한 것이다.
                {
                    exp_result ++;

                    //이경우 sfp범위 최소값에서 벗어남
                    if(exp_result ==31)
                        return 63<<10;

                    //이경우도 sfp범위에서 벗어남.(exp 하나 증가되면서 아래비트 잘리므로)-> rounding필요
                    if(frac_result %2 ==1) 
                    {
                        if((frac_result-1) % 4 ==0 ) //frac_result의 마지막에서 두번재 비트가 0이라는 뜻->이러면 최종 frac_result는 결국 짝수이다. 따라서 round to even에 의하여, 버림.
                            frac_result>>=1;
                        else  //이경우 마지막에서 두번째비트가 1이라는 뜻이다.->이러면 frac_result는 결국 홀수가되는데,  짝수로가야하므로, 하나더한다.
                        {    
                            frac_result>>=1;
                            frac_result++;
                            if (frac_result / (1 << 10) == 1) // 1 더해지면서 frac값이 또 overflow될수도 있음
                            {
                                exp_result++;
                                frac_result = 0; // frac부분이 1111111111 이기 때문에, 1 더해져서 영이됨.
                            }
                        }

                    }
                    else
                    	frac_result>>=1;
                }
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
                signbit_result = 1;
            } 
            else
            {
                frac_result = frac_a + (1<<10) - frac_b;
                signbit_result = 1;
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
                else
                {
                    while(1)
                    {
                        exp_result--;
                        frac_result <<= 1;
                        if(frac_result/ (1<<10)==1)
                        {   
                            frac_result -=(1<<10);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if(signbit_b ==0)
            {
                frac_result = frac_a + (1<<10) + frac_b;
                if(frac_result /(1<<10) >=2)//exp의 조정이 필요 a의 leading '1'과 frac_result가 이진법의 유효 10자리 보다 큰경우와 합쳐지는 경우를 조건문으로 제시한 것이다.
                {
                    exp_result ++;

                    //이경우 sfp범위 최대값에서 벗어남
                    if(exp_result ==31)
                        return 31<<10;

                    //이경우 아래비트 잘리므로)-> rounding필요
                    if(frac_result %2 ==1) 
                    {
                        if((frac_result-1)%4 ==0 ) //frac_result의 마지막에서 두번재 비트가 0이라는 뜻->이러면 최종 frac_result는 결국 짝수이다. 따라서 round to even에 의하여, 버림.
                            frac_result>>=1;
                        else  //이경우 마지막에서 두번째비트가 1이라는 뜻이다.->이러면 frac_result는 결국 홀수가되는데,  짝수로가야하므로, 하나더한다.
                        {    
                            frac_result>>=1;
                            frac_result++;
                            if (frac_result / (1 << 10) == 1) // 1 더해지면서 frac값이 또 overflow될수도 있음
                            {
                                exp_result++;
                                frac_result = 0; // frac부분이 1111111111 이기 때문에, 1 더해져서 영이됨.
                            }
                        }

                    }
                    else
                    	frac_result>>=1;
                }    
                signbit_result = 0;
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
            }
            else
            {
                frac_result = frac_a +(1<<10) - frac_b;
                signbit_result = 0;
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
                else
                {
                    while(1)
                    {
                        exp_result--;
                        frac_result <<= 1;
                        if(frac_result/ (1<<10)==1)
                        {   
                            frac_result -=(1<<10);
                            break;
                        }
                    }
                }
            }
        }
        		
    }
    else if(exp_a<exp_b)
    {
        int cnt = 0;
        float rounding = 0; //rounding 위한 저장 변수
        int rnd = 0;//rounding 위하여 rounding의 값의 정확한값을 알려주는 변수
        exp_result = exp_b;
        while(exp_b != exp_a)
        {
            exp_a++;

            //sfp 범위를 벗어나는 경우 round to even 실행
            if(frac_a%2 ==1)
            {
                rounding += 1<<rnd;
            }

            frac_a>>=1;
            if(cnt==0)
                frac_a += 1<<9; //leading '1'이 포함되어야함 처음한번만 포함해주면됨.
            cnt++;
            if(rounding >=1)
                rnd++; //rounding 위하면 반드시 필요 (비교해야하므로)
        }
        //--------------------------------a 의 round to even ----------------------------------//
        if(rounding>=1)
        {
            if (rounding / (1 << rnd) > 0.5) // rounding이 1/2 초과인지 미만인지 같은지 판단해야 to even할 수 있음
                frac_a += 1;
            else if (rounding / (1 << rnd) == 0.5)
            {
                if (frac_a % 2 == 1) //마지막비트가 홀수 (짝수일때는 그냥 버리면 됨.)
                    frac_a += 1;
            }
        }

        //--------------------------------a 의 round to even ----------------------------------//
        if(signbit_b ==1)
        {
            if(signbit_a ==1)
            {
                frac_result = frac_a + frac_b;
                if(frac_result /(1<<10) >=2)//exp의 조정이 필요 b의 leading '1'과 frac_result가 이진법의 유효 10자리 보다 큰경우와 합쳐지는 경우를 조건문으로 제시한 것이다.
                {
                    exp_result ++;

                    //이경우 sfp범위 최소값에서 벗어남
                    if(exp_result ==31)
                        return 63<<10;

                    //이경우도 sfp범위에서 벗어남.(아래비트 잘리므로)-> rounding필요
                    if(frac_result %2 ==1)
                    {
                        if((frac_result-1)%4 ==0 ) //frac_result의 마지막에서 두번재 비트가 0이라는 뜻->이러면 최종 frac_result는 결국 짝수이다. 따라서 round to even에 의하여, 버림.
                            frac_result>>=1;
                        else  //이경우 마지막에서 두번째비트가 1이라는 뜻이다.->이러면 frac_result는 결국 홀수가되는데,  짝수로가야하므로, 하나더한다.
                        {
                            frac_result>>=1;
                            frac_result++;
                            if (frac_result / (1 << 10) == 1) // 1 더해지면서 frac값이 또 overflow될수도 있음
                            {
                                exp_result++;
                                frac_result = 0; // frac부분이 1111111111 이기 때문에, 1 더해져서 영이됨.
                            }
                        }

                    }
                    else
                    	frac_result>>=1;
                }
                signbit_result = 1;
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
            }
            else
            {
                frac_result = frac_b + (1<<10) - frac_a;
                signbit_result = 1;
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
                else
                {
                    while(1)
                    {
                        exp_result--;
                        frac_result <<= 1;
                        if(frac_result/ (1<<10)==1)
                        {   
                            frac_result -=(1<<10);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if(signbit_a ==0)
            {
                frac_result = frac_b + (1<<10) +frac_a;
                if(frac_result /(1<<10) >=2)//exp의 조정이 필요 a의 leading '1'과 frac_result가 이진법의 유효 10자리 보다 큰경우와 합쳐지는 경우를 조건문으로 제시한 것이다.
                {
                    exp_result ++;

                    //이경우 sfp범위 최대값에서 벗어남
                    if(exp_result ==31)
                        return 31<<10;

                    //이경우도 sfp범위에서 벗어남.(아래비트 잘리므로)-> rounding필요
                    if(frac_result %2 ==1)
                    {
                        if((frac_result-1) %4 ==0 ) //frac_result의 마지막에서 두번재 비트가 0이라는 뜻->이러면 최종 frac_result는 결국 짝수이다. 따라서 round to even에 의하여, 버림.
                            frac_result>>=1;
                        else  //이경우 마지막에서 두번째비트가 1이라는 뜻이다.->이러면 frac_result는 결국 홀수가되는데,  짝수로가야하므로, 하나더한다.
                        {
                            frac_result>>=1;
                            frac_result++;
                            if (frac_result / (1 << 10) == 1) // 1 더해지면서 frac값이 또 overflow될수도 있음
                            {
                                exp_result++;
                                frac_result = 0; // frac부분이 1111111111 이기 때문에, 1 더해져서 영이됨.
                            }
                        }

                    }
                }
                signbit_result = 0;
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
            }
            else
            {
                frac_result = frac_b + (1<<10) - frac_a;
                signbit_result = 0;
                if (frac_result / (1 << 10) == 1)
                    frac_result -= 1 << 10; // leading 1 원상 복귀.
                else
                {
                    while(1)
                    {
                        exp_result--;
                        frac_result <<= 1;
                        if(frac_result/ (1<<10)==1)
                        {   
                            frac_result -=(1<<10);
                            break;
                        }
                    }
                }
            }
        }
    }
    //exp가 같을때
    else
    {
        exp_result = exp_a;
        if(signbit_a ==1)//a가 음수인 경우
        {
            if(signbit_b ==1)
            {
                frac_result = frac_a + frac_b + (1 << 10) + (1 << 10); //무조건 frac_result는 exp가 하나 늘어날 수밖에 없음 각각 leading 1이 존재하므로.
                exp_result++;

                //이경우 sfp범위 최소값에서 벗어남
                if (exp_result == 31)
                    return 63 << 10;

                //이경우도 sfp범위에서 벗어남.(아래비트 잘리므로)-> rounding필요
                if (frac_result % 2 == 1)
                {
                    if ((frac_result - 1) % 4 == 0) // frac_result의 마지막에서 두번재 비트가 0이라는 뜻->이러면 최종 frac_result는 결국 짝수이다. 따라서 round to even에 의하여, 버림.
                        frac_result >>= 1;
                    else //이경우 마지막에서 두번째비트가 1이라는 뜻이다.->이러면 frac_result는 결국 홀수가되는데,  짝수로가야하므로, 하나더한다.
                    {
                        frac_result >>= 1;
                        frac_result++;
                        if (frac_result / (1 << 10) == 1) // 1 더해지면서 frac값이 또 overflow될수도 있음
                        {
                            exp_result++;
                            frac_result = 0; // frac부분이 1111111111 이기 때문에, 1 더해져서 영이됨.
                        }
                    }
                }
                else
                {
                    frac_result>>=1;
                }
                signbit_result = 1;
                frac_result -=(1<<10);
            }
            else
            {
                if(frac_a>frac_b) //a의 절댓값이 b의 절댓값보다 크면 결과값은 음수이다.
                {
                    frac_result = -((1 << 10) + frac_a) + ((1 << 10) + frac_b); // leading 1 이 a 와 b 둘다 존재한다.
                    signbit_result = 0;
                    if (frac_result / (1 << 10) == 0)
                    {
                        for(int i = 9; i>=0 ; i--)
                        {   
                            if(frac_result / (1<<i) ==1)
                            {
                                exp_result--;
                                break;
                            }
                            frac_result <<= 1;
                            exp_result--;
                        }
                        frac_result <<= 1;
                    }
                    else
                    	frac_result-=1<<10;    
                }
                else if(frac_a<frac_b)
                {
                    frac_result = -((1 << 10) + frac_a) + ((1 << 10) + frac_b); // leading 1 이 a 와 b 둘다 존재한다.
                    signbit_result = 0;
                    if (frac_result / (1 << 10) == 0)
                    {
                        for(int i = 9; i>=0 ; i--)
                        {   
                            if(frac_result / (1<<i) ==1)
                            {
                                exp_result--;
                                break;
                            }
                            frac_result <<= 1;
                            exp_result--;
                        }
                        exp_result--;
                        frac_result <<= 1;
                    }
                    else
                    	frac_result-=1<<10;    
                }
                else //아예 같을 때
                {
                    return 0;
                }

            }
        }
        else
        {
            if(signbit_b ==0)
            {
                frac_result = frac_a + frac_b + (1 << 10) + (1 << 10);
                exp_result++;

                //이경우 sfp범위 최대값에서 벗어남
                if (exp_result == 31)
                    return 31 << 10;

                //이경우도 sfp범위에서 벗어남.(아래비트 잘리므로)-> rounding필요
                if (frac_result % 2 == 1)
                {
                    if ((frac_result-1) % 4== 0) // frac_result의 마지막에서 두번재 비트가 0이라는 뜻->이러면 최종 frac_result는 결국 짝수이다. 따라서 round to even에 의하여, 버림.
                        frac_result >>= 1;
                    else //이경우 마지막에서 두번째비트가 1이라는 뜻이다.->이러면 frac_result는 결국 홀수가되는데,  짝수로가야하므로, 하나더한다.
                    {
                        frac_result >>= 1;
                        frac_result++;
                        if (frac_result / (1 << 10) == 1) // 1 더해지면서 frac값이 또 overflow될수도 있음
                        {
                            exp_result++;
                            frac_result = 0; // frac부분이 1111111111 이기 때문에, 1 더해져서 영이됨.
                        }
                    }
                }
                else
                {
                    frac_result>>=1;
                }

                signbit_result = 0;
                frac_result -= 1 << 10; // leading 1 원상 복귀.
            }
            else //a는 양수 b는 음수
            {
                if (frac_a > frac_b) // a의 절댓값이 b의 절댓값보다 크면 결과값은 양수이다.
                {
                    frac_result = (1 << 10) + frac_a - ((1 << 10) + frac_b); // leading 1 이 a 와 b 둘다 존재한다.
                    signbit_result = 0;
                    if (frac_result / (1 << 10) == 0)
                    {
                        while(1)
                        {
                            for(int i = 9; i>=0 ; i--)
                            {   
                                if(frac_result / (1<<i) ==1)
                                {
                                    exp_result--;
                                    break;
                                }
                                frac_result <<= 1;
                            }
                            frac_result <<= 1;
                        }
                    }    
                }
                else if (frac_a < frac_b)
                {
                    frac_result = -((1 << 10) + frac_a) + ((1 << 10) + frac_b); // leading 1 이 a 와 b 둘다 존재한다.
                    signbit_result = 1;
                    if (frac_result / (1 << 10) == 0)
                    {
                        while(1)
                        {
                            for(int i = 9; i>=0 ; i--)
                            {   
                                if(frac_result / (1<<i) ==1)
                                {
                                    exp_result--;
                                    break;
                                }
                                frac_result <<= 1;
                            }
                            frac_result <<= 1;
                        }
                    }    
                }
                else //아예 같을 때
                {
                    return 0;
                }
            }
        }
    }
    c += (signbit_result<<15) + (exp_result<<10)+(frac_result);
    return c;
}

sfp sfp_mul(sfp a, sfp b){
	
    sfp c = 0;
    int signbit_a = 0; //a의 부호를 양으로 초기화
    int signbit_b = 0; //b의 부호를 양으로 초기화
    int signbit_result = 0;
    int exp_a;
    int exp_b;
    int exp_result = 0;
    if (a>>15 ==1)
    {   
        signbit_a = 1;
        exp_a = (a-(signbit_a * (1<<15))>>10);
    }
    else
        exp_a = (a>>10);
    if (b>>15 ==1)
    {   
        signbit_b = 1;
        exp_b = (b-(signbit_b * (1<<15))>>10);
    }
    else
        exp_b = (b>>10);
    if (signbit_a == 1)
    {
        if (signbit_b == 1)
            signbit_result = 0;
        else
            signbit_result = 1;
    }
    else
    {
        if (signbit_b == 1)
            signbit_result = 1;
        else
            signbit_result = 0;
    }
    int frac_a;
    int frac_b;
    int frac_result = 0;
    frac_a = a % (1<<10);
    frac_b = b % (1<<10);

    //------------------------예외 사항들-------------------------//
    if(exp_a ==31 && frac_a ==0 && signbit_a ==0)
    {
        if(exp_b ==31 && frac_b ==0
         && signbit_b ==0)
            return 31<<10; //무한대 (exp부분만 11111인경우)
        else if(exp_b==31 && frac_b ==0 && signbit_b ==1)
            return 63<<10;
        else if(exp_b <31)
            return (31<<10);  
        else if( b==0)
            return (31<<10) + 1;
    }
    else if(exp_a ==31 && frac_a ==0 && signbit_a ==1)
    {
        if(exp_b ==31 && frac_b ==0 && signbit_b ==1)
            return 31<<10; //무한대
        else if(exp_b ==31 && frac_b ==0 && signbit_b ==0)
            return 63<<10;
        else if(exp_b<31)
            return 63<<10;
        else if(b ==0)
            return (31<<10) +1;
    }
    else if(exp_a==31 && frac_a !=0)
        return (31<<10) + 1;
    //------------------------예외 사항 끝-------------------------//

    frac_result = ((1<<10)+frac_a) * ((1<<10)+frac_b); //실제 결과값 frac부분을 찾기위한 임시변수
    exp_result = exp_a + exp_b -15; //2의 지수부만 더하면 결국 결과값 지수부가된다. 하지만 이는 임시일뿐, fractmp_result에서 자릿수가 하나 더 올라갈 수 도 있다.
    int mod_power = 20;
    float rounding = 0; //rounding 위한 저장 변수
    int rnd = 0;

    //exp_result에 추가되는게 있는 지 확인하는 반복문
    while(1)
    {
        if(frac_result/(1<<mod_power) ==1) //사실상 1.xxxx 곱하기 1.xxxxx인데 소수점 아래 총 10자리이므로 곱셈하면 총 20자리의 소수자리가생김. 따라서 mod_power는 20이다.
        {
            break;
        }
        mod_power++;
        exp_result++;
    }

    rounding = frac_result%(1<<10);  //rounding value for base exp_result
    frac_result>>=10; //원래 frac의 자리에 맞춤.

    for (int i = mod_power - 20; i > 0; i--) //rounding for added exp_result by multiplication
    {
        if (frac_result % 2 == 1)
        {
            rounding += 1 << (rnd+10);
        }
        frac_result >>= 1;
        if (rounding >= 1)
            rnd++;
    }
    if(rounding>=1)
    {
        if(rounding/(1<<(rnd+10)) >0.5) 
        {
            frac_result += 1;
            if(frac_result/(1<<11)==1) //이전 frac_result의 모든비트가 다 1111111111 이면 exp_result증가하고, 모든비트 0으로초기화
            {
                exp_result ++;
                frac_result=0;
                frac_result>>=1;
            }
        }
        else if(rounding/ (1<<rnd) == 0.5)
        {
            if(frac_result %2 ==1)
            {   
                frac_result +=1;
                if (frac_result / (1 << 11) == 1) //이전 frac_result의 모든비트가 다 1111111111 이면 exp_result증가하고, 모든비트 0으로초기화
                {
                    exp_result++;
                    frac_result = 0;
                    frac_result>>=1;
                }
            }
        }

    }
    

    frac_result -=(1<<10);

    if(exp_result >31) //exp_result의 범위는 1부터 30이 유효한데, 그 이상이 되면, 수의 범위를 벗어난다. 31이 되면 denormalized value가 도출.
    {
        if(signbit_result ==1)
        {
            return 63<<10;
        }
        else
        {
            return 31<<10;
        }
    }
    else if(exp_result ==31)
    {
        if(frac_result == 0)
        {
            if(signbit_result ==0)
                return 31<<10;
            else
                return 63<<10;
        }
        else
        {
            return (31<<10) + 1;
        }
    }
    else
    {
        c += (signbit_result << 15) + (exp_result << 10) + (frac_result);
    }
    return c;
}

char* sfp2bits(sfp result){
    char* sfpbits_ans = malloc(sizeof(char)*17); //메모리 할당해 줘야함. 그래야 메인 함수에서 함수 반환 가능
    for(int i = 0; i<16; i++)
        sfpbits_ans[i] = '0';
    int n = 0;

    //sfp result에서 1이 존재하는 비트의 위치를 찾는 반복문
    int idx = 15;
    while(1)
    {
        n = result >> idx; 
        if(n!=0)
            break;
        idx--;
    }

    //그리고 sfpbits_ans 배열에 하나하나 비트를 넣는 반복문
    for (int j = 15-idx; j<16 ; j++)
    {
        sfpbits_ans[j] = (result >> idx) + 48; //문자열 이므로, 아스키코드값 변환 필수
        result -= ((sfpbits_ans[j] - 48)<<15-j);
        idx--;
    }
    return sfpbits_ans;

}

char* sfp_comp(sfp a, sfp b){

    char * comp1 = ">";
    char * comp2 = "=";
    char * comp3 = "<";

    //------------------------예외 사항들-------------------------//
    if(a==(31<<10))
    {
        if(b==(31<<10))
            return comp2;
        else if(b==(63<<10))
            return comp1;
        else
            return comp1;
    }
    else if(a==(63<<10))
    {
        if(b==(63<<10))
            return comp2;
        else
            return comp3;
    }
    else if(a/(1<<10)-(a/(1<<15)) ==31 && a%(1<<10)!=0)
        return comp2;
    //------------------------예외 사항 끝-------------------------//

    int signbit_a = 0; //a의 부호를 양으로 초기화
    int signbit_b = 0; //b의 부호를 양으로 초기화
    int exp_a;
    int exp_b;
    if (a>>15 ==1)
    {   
        signbit_a = 1;
        exp_a = (a-(signbit_a * (1<<15))>>10);
    }
    else
        exp_a = (a>>10);
    if (b>>15 ==1)
    {   
        signbit_b = 1;
        exp_b = (b-(signbit_b * (1<<15))>>10);
    }
    else
        exp_b = (b>>10);
    int frac_a;
    int frac_b;
    frac_a = a % (1<<10);
    frac_b = b % (1<<10);

    if(signbit_a ==1 && signbit_b ==0)
        return comp3;
    else if(signbit_a ==0 && signbit_b ==1)
        return comp1;
    else if(signbit_a ==1 && signbit_b ==1)
    {
        if(exp_a > exp_b)
            return comp3;
        else if(exp_a ==exp_b)
        {
            if(frac_a > frac_b)
                return comp3;
            else if(frac_a==frac_b)
                return comp2;
            else
                return comp1;
        }
        else
            return comp1;
    }
    else
    {
        if(exp_a>exp_b)
            return comp1;
        else if(exp_a ==exp_b)
        {
            if(frac_a > frac_b)
                return comp1;
            else if(frac_a== frac_b)
                return comp2;
            else
                return comp3;
        }
        else
            return comp3;
    }
}
