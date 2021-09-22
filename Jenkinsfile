pipeline {
    agent { dockerfile true }
    environment {
        TZ = 'Asia/Tokyo'
        NEXUS_CRED = credentials('Nexus')
        MVN_REPO_URL = "${env.MVN_REPO_URL}"
        MVN_REPO_USER_NAME = "${env.NEXUS_CRED_USR}"
        MVN_REPO_PASSWORD = "${env.NEXUS_CRED_PSW}"
    }
    
    stages {
        stage('Checkout') {
            steps {
                checkout scm
                
                script {
                    if (isUnix()) {
                        sh 'chmod +x ./gradlew'
                    }
                }
            }
        }
        stage('Build') {
            steps {
                sh './gradlew build'
            }
        }
        stage('Lint') {
            steps {
                recordIssues(tools: [androidLintParser(pattern: '**/lint-results*.xml')])
            }
        }
        stage('Deploy') {
            when {
                branch 'master'
            }
            steps {
                sh './gradlew uploadArchives'
            }
        }
    }
    post {
        success {
            archiveArtifacts 'build/**/*.aar'
        }
    }
}