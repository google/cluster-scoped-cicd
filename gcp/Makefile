USERNAME=en60573181su20
PASSWORD=en60573181su20
ACCOUNT=$(shell gcloud config get-value account)


REGION=us-east4
ZONE=us-east4-c
PROJECT=en60573181su20
VPC=k8s-vpc
SUBNET=$(VPC)-$(REGION)
CLUSTER=asylo-cluster
VERSION=1.16
IP=$(shell gcloud compute addresses list $(PROJECT) --format="value(address)")

KANIKO-SA=kaniko-sa
KANIKO-SA-FULL=$(KANIKO-SA)@$(PROJECT).iam.gserviceaccount.com

POSTGRES_USERNAME=en60573181su20
POSTGRES_PASSWORD=$(shell kubectl get secret --namespace default postgresql-postgresql-ha-postgresql -o jsonpath="{.data.postgresql-password}" | base64 --decode)

all: network cluster \
	cluster-kaniko cluster-tekton-pipelines cluster-tekton-dashboard \
	cluster-tekton-triggers cluster-nginx-ingress cluster-cert-manager \
	redis postgresql-db \
	harbor \
	pipeline-objects

config:
	gcloud config set project $(PROJECT)

enableapis: config
	gcloud services enable \
		compute.googleapis.com \
		container.googleapis.com \
		containerregistry.googleapis.com	

helm:
	curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/master/scripts/get-helm-3
	chmod 700 get_helm.sh
	./get_helm.sh
	-helm init

tkn:
	curl -LO https://github.com/tektoncd/cli/releases/download/v0.10.0/tkn_0.10.0_Linux_x86_64.tar.gz
	sudo tar xvzf tkn_0.10.0_Linux_x86_64.tar.gz -C /usr/local/bin/ tkn

network: config
	-gcloud compute networks create $(VPC) \
		--subnet-mode custom
	-gcloud compute networks subnets create $(SUBNET) \
		--network k8s-vpc \
		--range 10.0.0.0/16 \
		--region $(REGION)
	-gcloud compute firewall-rules create allow-http \
		--allow tcp:80 \
		--network $(VPC)
	-gcloud compute firewall-rules create allow-harbor \
		--allow tcp:30002-30003 \
		--network $(VPC)
	-gcloud compute addresses create $(PROJECT) --region $(REGION)

cluster:
	gcloud container clusters create $(CLUSTER) \
		--zone $(ZONE) \
		--num-nodes 3 \
		--machine-type n1-standard-4 \
		--cluster-version $(VERSION) \
		--network $(VPC) \
		--subnetwork $(SUBNET)
	gcloud container clusters get-credentials $(CLUSTER) \
		--zone $(ZONE)

postgresql:
	-helm repo add bitnami https://charts.bitnami.com/bitnami
	-helm install postgresql  \
		--set postgresql.username=en60573181su20 \
		--set postgresql.password=en60573181su20 \
		bitnami/postgresql-ha
	echo ${POSTGRES_PASSWORD}
	echo "TEST"

postgresql-db: postgresql
	sleep 60
	kubectl run postgresql-postgresql-ha-client --rm --tty -i --restart='Never' --namespace default --image bitnami/postgresql:11 \
		--env="PGPASSWORD=$(PASSWORD)" --command -- psql -h postgresql-postgresql-ha-pgpool -p 5432 -U en60573181su20 -d postgres \
		-c "CREATE DATABASE registry"
	kubectl run postgresql-postgresql-ha-client --rm --tty -i --restart='Never' --namespace default --image bitnami/postgresql:11 \
		--env="PGPASSWORD=$(PASSWORD)" --command -- psql -h postgresql-postgresql-ha-pgpool -p 5432 -U en60573181su20 -d postgres \
		-c "CREATE DATABASE clair"
	kubectl run postgresql-postgresql-ha-client --rm --tty -i --restart='Never' --namespace default --image bitnami/postgresql:11 \
		--env="PGPASSWORD=$(PASSWORD)" --command -- psql -h postgresql-postgresql-ha-pgpool -p 5432 -U en60573181su20 -d postgres \
		-c "CREATE DATABASE notary_server"
	kubectl run postgresql-postgresql-ha-client --rm --tty -i --restart='Never' --namespace default --image bitnami/postgresql:11 \
		--env="PGPASSWORD=$(PASSWORD)" --command -- psql -h postgresql-postgresql-ha-pgpool -p 5432 -U en60573181su20 -d postgres \
		-c "CREATE DATABASE notary_signer"

redis:
	-helm repo add bitnami https://charts.bitnami.com/bitnami
	helm install redis \
		--set usePassword=false \
		bitnami/redis

harbor:
	-helm repo add harbor https://helm.goharbor.io
	#helm fetch harbor/harbor --untar
	helm install harbor harbor/harbor --version 1.4.0 \
		--set externalURL=HTTPS://HARBOR.YOURDOMAIN.COM \
		--set expose.type=ingress \
		--set expose.loadBalancer.ports.httpPort=80 \
		--set expose.loadBalancer.ports.httpsPort=443 \
		--set expose.ingress.hosts.core=HARBOR.YOURDOMAIN.COM \
		--set expose.ingress.hosts.notary=NOTARY.YOURDOMAIN.COM \
		--set expose.ingress.annotations.'nginx\.ingress\.kubernetes\.io/proxy-request-buffering'='"off"' \
		--set expose.tls.secretName=harbor-cert \
		--set expose.nodePort.name=harbor \
		--set database.type=external \
		--set database.external.host=postgresql-postgresql-ha-pgpool.default.svc.cluster.local \
		--set database.external.username=en60573181su20 \
		--set database.external.password=en60573181su20 \
		--set redis.type=external \
		--set redis.external.host=redis-master.default.svc.cluster.local
	kubectl annotate ingress harbor-harbor-ingress \
		cert-manager.io/cluster-issuer=letsencrypt-prod

clean-harbor:
	helm delete harbor
	kubectl delete pvc -l chart=harbor
	
cluster-tekton-dashboard:
	kubectl apply -f \
		https://raw.githubusercontent.com/kubernetes/dashboard/v2.0.3/aio/deploy/recommended.yaml

cluster-kaniko:
	-gcloud iam service-accounts create $(KANIKO-SA)
	-gcloud projects add-iam-policy-binding $(PROJECT) \
		--member 'serviceAccount:$(KANIKO-SA-FULL)' \
		--role "roles/storage.admin"
	gcloud iam service-accounts keys create \
		--iam-account $(KANIKO-SA-FULL) \
		$(KANIKO-SA).key
	kubectl create secret generic $(KANIKO-SA)-secret \
		--from-file="./kaniko-sa.key"

cluster-tekton-pipelines:
	kubectl apply -f \
		https://storage.googleapis.com/tekton-releases/pipeline/latest/release.yaml
	#kubectl get pods --namespace tekton-pipelines --watch
	kubectl apply -f \
		https://storage.googleapis.com/tekton-releases/dashboard/latest/tekton-dashboard-release.yaml
	kubectl apply -f tekton-pv.yaml 

cluster-tekton-triggers:
	kubectl apply -f \
		https://storage.googleapis.com/tekton-releases/triggers/latest/release.yaml

cluster-nginx-ingress:
	kubectl create clusterrolebinding cluster-admin-binding \
		--clusterrole cluster-admin \
		--user $(ACCOUNT)
	kubectl apply -f \
		https://raw.githubusercontent.com/kubernetes/ingress-nginx/controller-v0.34.1/deploy/static/provider/cloud/deploy.yaml
	kubectl patch svc ingress-nginx-controller \
	 --namespace ingress-nginx \
		-p '{"spec": {"loadBalancerIP": "$(IP)"}}'

cluster-cert-manager:
	kubectl apply -f \
		https://github.com/jetstack/cert-manager/releases/download/v0.15.1/cert-manager.yaml

pipeline-objects:
	kubectl apply -f yaml/resources/git_resource.yaml
	kubectl apply -f yaml/resources/gcr_resource.yaml
	kubectl apply -f yaml/resources/harbor_resource.yaml
	kubectl apply -f yaml/resources/asylo_registry_resource.yaml
	#
	kubectl apply -f yaml/role.yaml
	kubectl apply -f yaml/rolebinding.yaml
	#
	kubectl apply -f _secrets/secret.yaml
	kubectl apply -f _secrets/serviceaccount.yaml
	kubectl apply -f _secrets/harbor-secret.yaml
	kubectl apply -f _secrets/harbor-secret-tekton.yaml
	kubectl apply -f _secrets/harbor-sa.yaml
	kubectl delete secret regcred
	kubectl create secret generic regcred \
		--from-file=.dockerconfigjson=_secrets/config.json \
		--type=kubernetes.io/dockerconfigjson
	#
	kubectl apply -f certs/clusterissuer-staging.yaml
	kubectl apply -f certs/clusterissuer-prod.yaml
	#
	kubectl apply -f yaml/task.yaml
	kubectl apply -f yaml/task-deploy.yaml
	kubectl apply -f yaml/pipeline.yaml
	#
	kubectl apply -f yaml/asylo/asylodeploy.yaml
	kubectl apply -f yaml/asylo/asylopipeline.yaml
	#
	kubectl apply -f yaml/triggers/el-ingress.yaml
	kubectl apply -f yaml/triggers/eventlistener.yaml
	kubectl apply -f yaml/triggers/asylo-eventlistener.yaml

clean:
	-gcloud container clusters delete $(CLUSTER) --zone $(ZONE) --quiet
	-gcloud compute firewall-rules delete allow-http --quiet
	-gcloud compute networks subnets delete $(SUBNET) --region $(REGION) --quiet
	-gcloud compute networks delete $(VPC) --quiet

.ONESHELL:
test:
	TEST=5
	echo ${TEST}
	echo "test"

