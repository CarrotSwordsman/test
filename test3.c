#version 330 core


// ����Դ����һ���ṹ��
struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

// �������������һ���ṹ��
struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float shininess;
};

in vec3 position;
in vec3 normal;

// �������
uniform vec3 eye_position;
// ��Դ
uniform Light light;
// �������
uniform Material material;

uniform int isShadow;

out vec4 fColor;

void main()
{
	if (isShadow == 1) {
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
			
		// �����ĸ���һ�������� N,V,L,R(��������H)
		vec3 N=normalize(normal);
		vec3 V=normalize(eye_position-position);
		vec3 L=normalize(light.position-position);
		vec3 R=reflect(-L,N);

		// ���������I_a
		vec4 I_a = light.ambient * material.ambient;

		// ����ϵ�������������I_d
		float diffuse_dot = max(dot(L, N), 0);
		vec4 I_d = diffuse_dot *  light.diffuse * material.diffuse;

		// ����ϵ���;��淴�����I_s
		float specular_dot_pow = pow(max(dot(R, V), 0), material.shininess);
		vec4 I_s = specular_dot_pow * light.specular * material.specular;

		// ע�������Դ�ڱ�����ȥ���߹�
		if( dot(L, N) < 0.0 ) {
		    I_s = vec4(0.0, 0.0, 0.0, 1.0);
		} 

		fColor = I_a + I_d + I_s;
		fColor.a = 1.0;
	}
}
