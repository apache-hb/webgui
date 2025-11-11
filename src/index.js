import * as ECS from '@aws-sdk/client-ecs';

addToLibrary({
    createEcsClient: function(config) {
        return new ECS.ECSClient(config);
    }
});
