# 쿠버네티스 환경 구성

<br/>

## VAGRANT

- 가상화 환경을 관리하고 프로비저닝하는 도구
- VirtualBox, VMware 등과 같은 가상화 소프트웨어와 함께 사용 가능

### VAGRANT 파일 예시

- 쿠버네티스와 도커의 버전을 명시하고, 마스터 노드와 워커 노드의 설정이 작성된 코드
- 코드 상세
  - `Vagrant.configure("2") do |config|`: API 버전을 2로 하겠다는 의미
  - `#{k8s_V[0..3]}`: k8s_V의 변수의 0~3 번째를 읽어들여라 => `1.20`

```shell
# -*- mode: ruby -*-
# vi: set ft=ruby :

## configuration variables ##
# max number of worker nodes
N = 3 
# each of components to install 
k8s_V = '1.20.2'           # Kubernetes 
docker_V = '19.03.14-3.el7' # Docker  
ctrd_V = '1.3.9-3.1.el7'   # Containerd 
## /configuration variables ##

Vagrant.configure("2") do |config|
  
  #=============#
  # Master Node #
  #=============#

    config.vm.define "WO-m-k8s-#{k8s_V[0..3]}" do |cfg|
      cfg.vm.box = "sysnet4admin/CentOS-k8s"
      cfg.vm.provider "virtualbox" do |vb|
        vb.name = "WO-m-k8s-#{k8s_V[0..3]}(github_SysNet4Admin)"
        vb.cpus = 2
        vb.memory = 1746
        vb.customize ["modifyvm", :id, "--groups", "/WO-k8s-SgMST-#{k8s_V[0..3]}(github_SysNet4Admin)"]
      end
      cfg.vm.host_name = "m-k8s"
      cfg.vm.network "private_network", ip: "192.168.1.10"
      cfg.vm.network "forwarded_port", guest: 22, host: 60010, auto_correct: true, id: "ssh"
      cfg.vm.synced_folder "../data", "/vagrant", disabled: true 
      cfg.vm.provision "shell", path: "k8s_env_build.sh", args: N
      cfg.vm.provision "shell", path: "k8s_pkg_cfg.sh", args: [ k8s_V, docker_V, ctrd_V ]
      #cfg.vm.provision "shell", path: "master_node.sh"
      cfg.vm.provision "shell", path: "k_cfg_n_git_clone.sh" # add kubectl config & git clone source
    end

  #==============#
  # Worker Nodes #
  #==============#

  (1..N).each do |i|
    config.vm.define "WO-w#{i}-k8s-#{k8s_V[0..3]}" do |cfg|
      cfg.vm.box = "sysnet4admin/CentOS-k8s"
      cfg.vm.provider "virtualbox" do |vb|
        vb.name = "WO-w#{i}-k8s-#{k8s_V[0..3]}(github_SysNet4Admin)"
        vb.cpus = 1
        vb.memory = 1024
        vb.customize ["modifyvm", :id, "--groups", "/WO-k8s-SgMST-#{k8s_V[0..3]}(github_SysNet4Admin)"]
      end
      cfg.vm.host_name = "w#{i}-k8s"
      cfg.vm.network "private_network", ip: "192.168.1.10#{i}"
      cfg.vm.network "forwarded_port", guest: 22, host: "6010#{i}", auto_correct: true, id: "ssh"
      cfg.vm.synced_folder "../data", "/vagrant", disabled: true
      cfg.vm.provision "shell", path: "k8s_env_build.sh", args: N
      cfg.vm.provision "shell", path: "k8s_pkg_cfg.sh", args: [ k8s_V, docker_V, ctrd_V ]
      #cfg.vm.provision "shell", path: "work_nodes.sh"
    end
  end

end
```

<br/>

## k8s env build

- 아래는 쿠버네티스를 설치하기 위한 환경을 구성하는 파일이다. 
- `echo 'alias vi=vim' >> /etc/profile`: vi를 호출했을 때 vim을 호출하도록 수정
- `swapoff -a`: k8s 1.22 버전이 나오기 전까지는 스왑을 끄지 않으면 k8s가 설치되지 않았다고 한다. 어쨌든 일반 디스크에 스왑을 걸어놓으면 성능적 이슈가 발생하기 때문에 스왑을 끄도록 하자.
- `sed -i.bak -r 's/(.+ swap .+)/#\1/' /etc/fstab`: 리부팅을 하더라도 스왑을 키지 않도록 한다.

```shell
#!/usr/bin/env bash

# vim configuration 
echo 'alias vi=vim' >> /etc/profile

# swapoff -a to disable swapping
swapoff -a
# sed to comment the swap partition in /etc/fstab
sed -i.bak -r 's/(.+ swap .+)/#\1/' /etc/fstab

# kubernetes repo
gg_pkg="http://mirrors.aliyun.com/kubernetes/yum" # Due to shorten addr for key
cat <<EOF > /etc/yum.repos.d/kubernetes.repo
[kubernetes]
name=Kubernetes
baseurl=${gg_pkg}/repos/kubernetes-el7-x86_64
enabled=1
gpgcheck=0
repo_gpgcheck=0
gpgkey=${gg_pkg}/doc/yum-key.gpg ${gg_pkg}/doc/rpm-package-key.gpg
EOF

# add docker-ce repo
yum install yum-utils -y 
yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo

# Set SELinux in permissive mode (effectively disabling it)
setenforce 0
sed -i 's/^SELINUX=enforcing$/SELINUX=permissive/' /etc/selinux/config

# RHEL/CentOS 7 have reported traffic issues being routed incorrectly due to iptables bypassed
cat <<EOF >  /etc/sysctl.d/k8s.conf
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
EOF
modprobe br_netfilter

# local small dns & vagrant cannot parse and delivery shell code.
echo "192.168.1.10 m-k8s" >> /etc/hosts
for (( i=1; i<=$1; i++  )); do echo "192.168.1.10$i w$i-k8s" >> /etc/hosts; done

# config DNS  
cat <<EOF > /etc/resolv.conf
nameserver 1.1.1.1 #cloudflare DNS
nameserver 8.8.8.8 #Google DNS
EOF
```

<br/>

## k8s package config

- k8s 패키지 설정 파일
- 유틸성 패키지와 도커, k8s 등을 설치한다.

```shell
#!/usr/bin/env bash

# install util packages 
yum install epel-release -y
yum install vim-enhanced -y
yum install git -y

# install docker 
yum install docker-ce-$2 docker-ce-cli-$2 containerd.io-$3 -y

# install kubernetes
# both kubelet and kubectl will install by dependency
# but aim to latest version. so fixed version by manually
yum install kubelet-$1 kubectl-$1 kubeadm-$1 -y 

# Ready to install for k8s 
systemctl enable --now docker
systemctl enable --now kubelet
```

<br/>

## 실습 편의성

- `bash-completion` 패키지: 명령어 자동 완성


```shell
#!/usr/bin/env bash

# install bash-completion for kubectl 
yum install bash-completion -y 

# kubectl completion on bash-completion dir
kubectl completion bash >/etc/bash_completion.d/kubectl

# alias kubectl to k 
echo 'alias k=kubectl' >> ~/.bashrc
echo 'complete -F __start_kubectl k' >> ~/.bashrc

# git clone k8s-code
git clone https://github.com/sysnet4admin/_Lecture_k8s_learning.kit.git
mv /home/vagrant/_Lecture_k8s_learning.kit $HOME
find $HOME/_Lecture_k8s_learning.kit -regex ".*\.\(sh\)" -exec chmod 700 {} \;
```
